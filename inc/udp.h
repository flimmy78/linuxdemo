/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * COPYRIGHT NOTICE. 
 * Copyright (c), 2015 Lierda.
 * All rights reserved.
 * 
 * @file udp.
 * @brief 
 * 
 * @version 1.0
 * @author au
 * @date 2015/06/18-21:40:01
 * @email dongliang@lierda.com
 * 
 * @revision:
 *  - 1.0 2015/06/18 by au.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __UDP_H_
#define __UDP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

    namespace util {
        class Udp {
        public:
            Udp(int _port = 10000);
            ~Udp();
            bool create();
            void destroy();
            int send(char *_buf, unsigned int _size, 
                     char *_ip, int _port, int _s, int _u);
            int recv(char *_buf, unsigned int _size,
                     char *_ip, int *_port, int _s, int _u);
        private:
            int port;
            int fd;
        };

        Udp::Udp(int _port) :
        port(_port), fd(-1) {
        }
        Udp::~Udp() {
            destroy();
        }
        void Udp::destroy() {
            if (fd > 0) {
                ::close(fd);
                fd = -1;
            }
        }
        bool Udp::create() {
            int 				reuse = 1;
            struct sockaddr_in 	sa;
            int 				ret;

            ret = socket(AF_INET, SOCK_DGRAM, 0);
            if (ret < 0) {
                return false;
            }
            fd = ret;

            setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, 
                       sizeof(reuse));
            setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &reuse, 
                       sizeof(reuse));

            sa.sin_family = AF_INET;
            sa.sin_port   = htons((short)port);
            sa.sin_addr.s_addr = INADDR_ANY;
            //inet_aton("192.168.1.200", &(sa.sin_addr));

            ret = ::bind(fd, (struct sockaddr *)&sa, sizeof(sa));
            if (ret < 0) {
                destroy();
                return false;
            }
            return true;
        }

        int Udp::send(char *_buf, unsigned int _size, 
                      char *_ip, int _port, int _s, int _u) {
            fd_set			fds;
            struct timeval	tv;
            int 			ret;
            int 			en;
            char            es[256];
            struct sockaddr_in sa;
            socklen_t		sl = sizeof(sa);


            if (_buf == NULL || _size <= 0 || _ip == NULL ||
                _s < 0 || _u < 0)  {
                return -1;
            }

        send_select_tag:
            tv.tv_sec = _s;
            tv.tv_usec = _u;

            FD_ZERO(&fds);
            FD_SET(fd, &fds);

            sa.sin_family 	= AF_INET;
            sa.sin_port		= htons((short)_port);
            inet_aton(_ip, &(sa.sin_addr));

            ret = select(fd + 1, NULL, &fds, NULL, &tv);
            if (ret < 0) {
                en = errno;
                if (en == EAGAIN || en == EINTR) {
                    goto send_select_tag;
                } else {
                    strerror_r(en, es, sizeof(es));
                    debug_error("[Udp::send@select]:%d,%s\n", en, es);
                    return -2;
                }
            } else if (ret == 0) {
                return 0;
            } else if (FD_ISSET(fd, &fds)) {
            send_tag:
                ret = sendto(fd, _buf, _size, 0, (struct sockaddr *)&sa, sl);
                if (ret < 0) {
                    en = errno;
                    if (en == EAGAIN || en == EINTR) {
                        goto send_tag;
                    } else {
                        //perror("error");
                    strerror_r(en, es, sizeof(es));
                    debug_error("[Udp::send@send]:%d,%s\n", en, es);	
                    return -3;
                    }
                } else if (ret != (int)_size) {
                    debug_error("[Udp::send@]:ret != size\n");	
                    return -4;
                } else {
                    ;
                }
            } else {
                debug_error("[Udp::send@]:unknow!\n");	
                return -5;
            }

            return ret;
        }
        int Udp::recv(char *_buf, unsigned int _size,
                      char *_ip, int *_port, int _s, int _u) {
            fd_set				fds;
            struct timeval		tv;
            int 				ret;
            int 				en;
            char                es[256];
            struct sockaddr_in	sa;
            socklen_t		sl = sizeof(sa);

            if (_buf == NULL || _size <= 0 ||
                _s < 0 || _u < 0)  {
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
                    debug_error("[Udp::recv@select]:%d,%s\n", en, es);
                    return -2;
                }
            } else if (ret == 0) {
                return 0;
            } else if (FD_ISSET(fd, &fds)) {
            recv_tag:
                ret = recvfrom(fd, _buf, _size, 0, (struct sockaddr *)&sa, &sl);
                if (ret < 0) {
                    en = errno;
                    if (en == EAGAIN || en == EINTR) {
                        goto recv_tag;
                    } else {
                        strerror_r(en, es, sizeof(es));
                        debug_error("[Udp::recv@recv]:%d,%s\n", en, es);
                        return -3;
                    }
                } else {
                    if (ret == 0) {
                        debug_error("[Udp::recv@]:socket failed!\n");
                        return -4; //remote close the socket
                    } else {
                        if (_ip != NULL) {
                            strcpy(_ip, inet_ntoa(sa.sin_addr));
                        }

                        if (_port != NULL) {
                            *_port = (int)ntohs(sa.sin_port);
                        }
                    }
                }
            } else {
                debug_error("[Udp::recv@]:unknown\n");
                return -5;
            }
            return ret;
        }
    };
    
    



#ifdef __cplusplus
}
#endif
#endif

