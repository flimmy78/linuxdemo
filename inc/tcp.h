/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * COPYRIGHT NOTICE. 
 * Copyright (c), 2015 Lierda.
 * All rights reserved.
 * 
 * @file tcp.
 * @brief 
 * 
 * @version 1.0
 * @author au
 * @date 2015/06/15-15:31:37
 * @email dongliang@lierda.com
 * 
 * @revision:
 *  - 1.0 2015/06/15 by au.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __TCP_H_
#define __TCP_H_
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <errno.h>

#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif
    namespace util {
        class Tcp {
        public:
            enum {
                TCP_SERVER = 0,
                TCP_CLIENT = 1,
                TCP_PROXY  = 2,
            };
        public:
            Tcp(int _type = 0, /**> 0->server, 1->client */
                std::string _ip = "0.0.0.0",
                int _port = 10000);
            ~Tcp();
            bool create();
            void destroy();
            int recv(char *_buf, unsigned int _size, int _s, int _u);
            int send(char *_buf, unsigned int _size, int _s, int _u);
            int accept(int _s, int _u);
            void setfd(int _fd);
			int getfd() const { return fd; }
        private:
            int type;
            std::string ip;
            int port;
            int fd;
        };
        
        class TcpProxy : public Tcp {
        public:
            TcpProxy(int _fd);
            ~TcpProxy();
        };

        TcpProxy::TcpProxy(int _fd) : Tcp(TCP_PROXY, "", 0) {
            setfd(_fd);
        }
        TcpProxy::~TcpProxy() {
            destroy();
        }


        
        Tcp::Tcp(int _type, 
                 std::string _ip,
                 int _port) :
        type(_type), ip(_ip), port(_port), fd(-1) {
        }
        Tcp::~Tcp() {
            destroy();
        }
        void Tcp::setfd(int _fd) {
            fd = _fd;
        }
        bool Tcp::create() {
            int 				reuse = 1;
            struct sockaddr_in 	sa;
            int 				ret;
	
            ret = socket(AF_INET, SOCK_STREAM, 0);
            if (ret < 0) {
                return false;
            }
            fd = ret;

            setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
            //TODO: set more, if need

            if (type == TCP_CLIENT) {
                sa.sin_family = AF_INET;
                sa.sin_port   = htons((short)port);
                inet_aton(ip.c_str(), &(sa.sin_addr));
                ret = connect(fd, (struct sockaddr *)&sa, sizeof(sa));
                if (ret < 0) {
                    destroy();
                    return false;
                }
            } else if (type == TCP_SERVER) {
                sa.sin_family = AF_INET;
                sa.sin_port   = htons((short)port);
                inet_aton(ip.c_str(), &(sa.sin_addr));
                //sa.sin_addr.s_addr = INADDR_ANY;
                ret = ::bind(fd, (struct sockaddr *)&sa, sizeof(sa));
                if (ret < 0) {
                    destroy();
                    return false;
                }

                ret = listen(fd, 5);
                if (ret != 0) {
                    destroy();
                    return false;
                }
            }
            return true;
        }
        void Tcp::destroy() {
						if (!(fd > 0)) {
							return;
						}
						struct linger {
							int   l_onoff;
							int    l_linger;
						};
						//printf("----------- 2\n");
						char buf[128];
						struct linger linger = { 1, 0}; 
						setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char *) &linger, sizeof(linger));
						shutdown(fd, SHUT_RDWR);
						//printf("----------- 3\n");
						do { 
						//printf("----------- 4\n");
								int ret = ::recv(fd, buf, sizeof(buf), 0);
								if (ret == 0)  break;
								if (ret == -1) {	
									if (errno == EAGAIN) break;
									if (errno == EWOULDBLOCK) break;
									if (errno == 9) break;
									//printf("errno is %d\n", errno);
								}
						//printf("----------- 5 : ret: %d\n", ret);
						} while (1);
						//printf("----------- 6\n");
            ::close(fd);
						fd = -1;
						//printf("----------- 7\n");
        }

        int Tcp::recv(char *_buf, unsigned int _size, int _s, int _u) {
            fd_set	fds;
            struct timeval	tv;
            int ret;
            int en;
            char es[256];

            if (_buf == NULL || _size <= 0 || _s < 0 || _u < 0 || fd <= 0)  {
                return -1;
            }
        recv_select_tag:
            FD_ZERO(&fds);
            FD_SET(fd, &fds);
            tv.tv_sec = _s;
            tv.tv_usec = _u;
            ret = select(fd + 1, &fds, NULL, NULL, &tv);
            if (ret < 0) {
                en = errno;
                if (en == EAGAIN || en == EINTR) {
                    goto recv_select_tag;
                } else {
                    strerror_r(en, es, sizeof(es));
                    debug_error("[Tcp::recv@select]:%d,%s\n", en, es);
                    return -2;
                }
            } else if (ret == 0) {
                return 0;
            } else if (FD_ISSET(fd, &fds)) {
            recv_tag:
                ret = ::recv(fd, _buf, _size, 0);
                if (ret < 0) {
                    en = errno;
                    if (en == EAGAIN || en == EINTR) {
                        goto recv_tag;
                    } else {
                        strerror_r(en, es, sizeof(es));
                        debug_error("[Tcp::recv@recv]:%d,%s\n", en, es);
                        return -3;
                    }
                } else {
                    if (ret == 0) {
                        debug_error("[Tcp::recv@]:remote socket closed!\n");
                        return -4; //remote close the socket
                    } else {
                        ;
                    }
                }
            } else {
                debug_error("[Tcp::recv@]:unknown\n");
                return -5;
            }
            return ret;
        }

        int Tcp::send(char *_buf, unsigned int _size, int _s, int _u) {
            fd_set	fds;
            struct timeval tv;
            int ret;
            int en;
            char es[256];
            int try_count = 5;


            if (_buf == NULL || _size <= 0 || _s < 0 || _u < 0 || fd <= 0)  {
                return -1;
            }

        send_select_tag:
            FD_ZERO(&fds);
            FD_SET(fd, &fds);
            tv.tv_sec = _s;
            tv.tv_usec = _u;
            ret = select(fd + 1, NULL, &fds, NULL, &tv);
            if (ret < 0) {
                en = errno;
                if (en == EAGAIN || en == EINTR) {
                    goto send_select_tag;
                } else {
                    strerror_r(en, es, sizeof(es));
                    debug_error("[Tcp::send@select]:%d,%s\n", en, es);	
                    return -2;
                }
            } else if (ret == 0) {
                return 0;
            } else if (FD_ISSET(fd, &fds)) {
            send_tag:
                ret = ::send(fd, _buf, _size, 0);
                if (ret < 0) {
                    en = errno;
                    if ((en == EAGAIN || en == EINTR) && (try_count-- >= 0)) {
                        usleep(10);
                        goto send_tag;
                    } else {
                        strerror_r(en, es, sizeof(es));
                        debug_error("[Tcp::send@send]:%d,%s\n", en, es);	
                        return -3;
                    }
                } else if (ret != (int)_size) {
                    debug_error("[Tcp::send@]:ret != size\n");	
                    return -4;
                } else {
                    ;
                }
            } else {
                debug_error("[Tcp::send@]:unknow!\n");	
                return -5;
            }
            return ret;
        }

        int Tcp::accept(int _s, int _u) {
            fd_set	fds;
            struct timeval	tv;
            int ret;
            struct sockaddr_in sa;
            socklen_t		sl = sizeof(sa);
            int en;
            char es[256];

        accept_select_tag:
            FD_ZERO(&fds);
            FD_SET(fd, &fds);
            tv.tv_sec = _s;
            tv.tv_usec = _u;

            ret = select(fd + 1, &fds, NULL, NULL, &tv);
            if (ret < 0) {
                en = errno;
                if (en == EAGAIN || en == EINTR) {
                    goto accept_select_tag;
                } else {
                    strerror_r(en, es, sizeof(es));
                    debug_error("[Tcp::accept@select]:%d,%s\n", en, es);	
                    return -1;
                }
            } else if (ret == 0) {
                return 0;
            } else if (FD_ISSET(fd, &fds)) {
            accept_tag:
                ret = ::accept(fd, (struct sockaddr *)&sa, &sl);
                if (ret < 0) {
                    en = errno;
                    if (en == EAGAIN || en == EINTR) {
                        goto accept_tag;
                    } else {
                        strerror_r(en, es, sizeof(es));
                        debug_error("[Tcp::accept@accept]:%d,%s\n", en, es);	
                        return -2;
                    }
                } else{
                    ;
                }
            } else {
                debug_error("[Tcp::accept@]:unknown!\n");	
                return -3;
            }
            return ret;
        }
    };
#ifdef __cplusplus
}
#endif
#endif
