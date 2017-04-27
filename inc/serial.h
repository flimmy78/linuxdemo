/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * COPYRIGHT NOTICE. 
 * Copyright (c), 2015 Lierda.
 * All rights reserved.
 * 
 * @file serial.
 * @brief 
 * 
 * @version 1.0
 * @author au
 * @date 2015/06/15-08:22:51
 * @email dongliang@lierda.com
 * 
 * @revision:
 *  - 1.0 2015/06/15 by au.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __SERIAL_H_
#define __SERIAL_H_
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif
    namespace util {
        class Serial {
        public:
            Serial(std::string _dev = "/dev/ttyATH0", 
                   int _buad = 9600,
                   char _nbits = 8,
                   char _parity = 'N',
                   char _stop = 1);
            ~Serial();
            bool open();
            void close();
            int write(char *_buf, unsigned int _size, int _s, int _u);
            int read(char *_buf, unsigned int _size, int _s, int _u);
            void flush();
            bool setfmt(int _buad, char _nbits, char _parity, char _stop);
			int getfd() { return fd; }
        private:
            int _read(char *_c, int _s, int _u);
            void _setTermios(struct termios * _pNewtio, int _buat, char _nbits, char _parity, char _stop);
            std::string device;
            int baudrate;
            char nbits;
            char parity;
            char stop;
            int fd;
        };
        Serial::Serial(std::string _dev,
                       int _buad,
                       char _nbits,
                       char _parity,
                       char _stop) :
        device(_dev), baudrate(_buad), nbits(_nbits), 
            parity(_parity), stop(_stop), fd (-1) {
								std::cout << _dev << std::endl;
        }
        Serial::~Serial() {
            close();
        }
        bool Serial::open() {
            int ret;

            ret = ::open(device.c_str(), O_RDWR | O_NOCTTY);
            if (ret < 0) {
								cout << "111111111111111" << endl;
                return false;
            }
            fd = ret;
	
            if (!setfmt(baudrate, nbits, parity, stop)) {
                /**> TODO: check here, if need to close */
								cout << "111111111111112" << endl;
                close();
                return false;
            }

            return true;
        }

        void Serial::close() {
            if (fd > 0) {
                ::close(fd);
                fd = -1;
            }
        }

        int Serial::write(char *_buf, unsigned int _size, int _s, int _u) {
            int 	ret;
            int 	en;
            fd_set	fds;
            struct	timeval tv;
            char    es[256];

            if (_buf  == NULL || _size <= 0 || _s < 0 || _u < 0 || fd < 0) {
                return -1;
            }
        write_select_tag:
            FD_ZERO(&fds);
            FD_SET(fd, &fds);
            tv.tv_sec = _s;
            tv.tv_usec = _u;
            ret = ::select(fd + 1, NULL, &fds, NULL, &tv);
            if (ret < 0) {
                en = errno;
                if (en == EAGAIN || en == EINTR) {
                    goto write_select_tag;
                } else { //if (en == EBADF || en == EINVAL || en == ENOMEM) {
                    strerror_r(en, es, sizeof(es));
                    debug_error("[Serial::write@select]:%d,%s\n", en, es);
                    return -2;
                }
            } else if (ret == 0) {
                return 0;
            } else if (FD_ISSET(fd, &fds)) {
            write_tag:
                ret = ::write(fd, _buf,  _size);
                if (ret < 0) {
                    en = errno;
                    if (en == EAGAIN || en == EINTR) {
                        goto write_tag;
                    } else {
                        strerror_r(en, es, sizeof(es));
                        debug_error("[Serial::write@write]:%d,%s\n", en, es);
                        return -3;
                    }
                } else if ((unsigned)ret != _size) {
                    debug_error("[Serial::write@]:ret != size\n");
                    return -4;
                } else {
                    ;
                } 
            } else {
                debug_error("[Serial::write@]:unknown\n");
                return -5;
            }
            return ret;
        }

        int Serial::read(char *_buf, unsigned int _size, int _s, int _u) {
            int ret;
            unsigned int i;

            if (_buf  == NULL || _size <= 0 || _s < 0 || _u < 0 || fd < 0) {
                return -1;
            }
	
            for (i = 0; i < _size; i++) {
                if (i == 0) 
                    ret = _read(&(_buf[i]), _s, _u);
                else 	
                    ret = _read(&(_buf[i]), 0, _u);
                if (ret == 1) {
                    ;
                } else if (ret == 0) {
                    break;
                } else {
                    return ret;
                }
            }

            return i;
        }

        void Serial::flush() {
            tcflush(fd, TCIFLUSH);
            tcflush(fd, TCOFLUSH);
        }

        bool Serial::setfmt(int _buad, char _nbits, char _parity, char _stop) {
            struct termios newtio;
            //struct termios oldtio;
            int	ret;
            
            if (fd < 0) {
                return false;
            }

            //tcgetattr(fd, &oldtio);
            _setTermios(&newtio, _buad, _nbits, _parity, _stop);
            tcflush(fd, TCIFLUSH);
            tcflush(fd, TCOFLUSH);
            ret = tcsetattr(fd, TCSANOW, &newtio);
            if (ret < 0) {
                return false;
            }
            return true;
        }


        int Serial::_read(char *_c, int _s, int _u) {
            int 	ret;
            int 	en;
            char    es[256];
            char	ch;

	
            fd_set	fds;
            struct	timeval tv;
	
            if (_c == NULL || _s < 0 || _u < 0 || fd < 0) {
                return -1;
            }
        _read_select_tag:	
            FD_ZERO(&fds);
            FD_SET(fd, &fds);
            tv.tv_sec = _s;
            tv.tv_usec = _u;
            ret = select(fd + 1, &fds, NULL, NULL, &tv);
            if (ret < 0) {
                en = errno;
                if (en == EAGAIN || en == EINTR) {
                    goto _read_select_tag;
                } else { //if (en == EBADF || en == EINVAL || en == ENOMEM) {
                    strerror_r(en, es, sizeof(es));
                    debug_error("[Serial::_read@select]:%d,%s\n", en, es);
                    return -3;
                }
            } else if (ret == 0) {
                return 0;
            } else if (FD_ISSET(fd, &fds)) {
            read_tag:
                ret = ::read(fd, &ch, 1);
                if (ret < 0) {
                    en = errno;
                    if (en == EAGAIN || en == EINTR) {
                        goto read_tag;
                    } else { 
                        strerror_r(en, es, sizeof(es));
                        debug_error("[Serial::_read@read]:%d,%s\n", en, es);
                        return -4;
                    }
                } else if (ret != 1) {
                    debug_error("[Serial::_read@]:ret != 1\n");
                    return -5;
                } else {
                    ;
                } 
            } else {
                debug_error("[Serial::_read@]:unknown\n");
                return -6;
            }
	
            *_c = ch;
            return ret;
        }


        void Serial::_setTermios(struct termios * _pNewtio, int _buat, char _nbits, char _parity, char _stop) {
            int uBaudRate;
            switch(_buat) {
            case 1200:
                uBaudRate = B1200;
                break;
            case 2400:
                uBaudRate = B2400;
                break;
            case 4800:
                uBaudRate = B4800;
                break;
            case 9600:
                uBaudRate = B9600;
                break;
            case 19200:
                uBaudRate = B19200;
                break;
            case 38400:
                uBaudRate = B38400;
                break;
            case 57600:
                uBaudRate = B57600;
                break;
            case 115200:
                uBaudRate = B115200;
                break;
            default:
                uBaudRate = B115200;
                break;
            }
            bzero(_pNewtio, sizeof(struct termios));

            printf("buat : %d, nbits: %d, parity: %c, stop: %d\n", _buat, _nbits, _parity, _stop);
            _pNewtio->c_cflag 	 		 = CLOCAL | CREAD;
            _pNewtio->c_cflag	 		|= uBaudRate;

            switch (_nbits) {
            case 8:
                _pNewtio->c_cflag 	|= CS8;
                break;
            case 7: 
                _pNewtio->c_cflag	|= CS7;
                break;
            case 6: 
                _pNewtio->c_cflag	|= CS6;
                break;
            case 5: 
                _pNewtio->c_cflag	|= CS5;
                break;
            }

            if (_stop == 1) {
                ;
            } else {
                _pNewtio->c_cflag		|= CSTOPB;
            }
            
            if (_parity == 'N') {
                ;
            } else if (_parity == 'E') {
                _pNewtio->c_cflag		|= PARENB;
            } else if (_parity == 'O') {
                _pNewtio->c_cflag		|= PARENB | PARODD;
            } else {
                ;
            }

            //pNewtio->c_iflag = IGNPAR;
            _pNewtio->c_iflag = IGNPAR;
            //pNewtio->c_iflag = 0;

            _pNewtio->c_oflag = 0;

            _pNewtio->c_lflag = 0; //non ICANON

            _pNewtio->c_cc[VINTR] = 0; 
            _pNewtio->c_cc[VQUIT] = 0;
            _pNewtio->c_cc[VERASE] = 0;
            _pNewtio->c_cc[VKILL] = 0;
            _pNewtio->c_cc[VEOF] = 0;
            _pNewtio->c_cc[VTIME] = 5; //5 *100ms
            _pNewtio->c_cc[VMIN] = 0; //0  最小等待字节数
            _pNewtio->c_cc[VSWTC] = 0;
            _pNewtio->c_cc[VSTART] = 0;
            _pNewtio->c_cc[VSTOP] = 0;
            _pNewtio->c_cc[VSUSP] = 0;
            _pNewtio->c_cc[VEOL] = 0;
            _pNewtio->c_cc[VREPRINT] = 0;
            _pNewtio->c_cc[VDISCARD] = 0;
            _pNewtio->c_cc[VWERASE] = 0;
            _pNewtio->c_cc[VLNEXT] = 0;
            _pNewtio->c_cc[VEOL2] = 0;
        }
    };
#ifdef __cplusplus
}
#endif
#endif
