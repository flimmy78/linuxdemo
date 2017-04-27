/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* COPYRIGHT NOTICE. 
* Copyright (c), 2015 Lierda.
* All rights reserved.
* 
* @file ExtFd.h.
* @brief 
* 
* @version 1.0
* @author au
* @date 2016/02/18-15:49:55
* @email dongliang@lierda.com
* 
* @revision:
*  - 1.0 2016/02/18by au.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
#ifndef __EXTFD_H_
#define __EXTFD_H_
#ifdef __cplusplus
extern "C" {
#endif

  namespace util {
	class ExtFd {
	public:
	ExtFd(int _fd) : fd(_fd) {}
	  virtual ~ExtFd() {}
	  int write(char *_buf, unsigned int _size, int _s, int _u);
	  int read(char *_buf, unsigned int _size, int _s, int _u);
	private:
	  int fd;
	  int _read(char *_c, int _s, int _u);
	};

	int ExtFd::write(char *_buf, unsigned int _size, int _s, int _u) {
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
		  debug_error("[ExtFd::write@select]:%d,%s\n", en, es);
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
			debug_error("[ExtFd::write@write]:%d,%s\n", en, es);
			return -3;
		  }
		} else if ((unsigned)ret != _size) {
		  debug_error("[ExtFd::write@]:ret != size\n");
		  return -4;
		} else {
		  ;
		} 
	  } else {
		debug_error("[ExtFD::write@]:unknown\n");
		return -5;
	  }
	  return ret;
	}

	int ExtFd::read(char *_buf, unsigned int _size, int _s, int _u) {
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

	int ExtFd::_read(char *_c, int _s, int _u) {
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
		  debug_error("[ExtFd::_read@select]:%d,%s\n", en, es);
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
			debug_error("[ExtFd::_read@read]:%d,%s\n", en, es);
			return -4;
		  }
		} else if (ret != 1) {
		  debug_error("[ExtFd::_read@]:ret != 1\n");
		  return -5;
		} else {
		  ;
		} 
	  } else {
		debug_error("[ExtFd::_read@]:unknown\n");
		return -6;
	  }
	
	  *_c = ch;
	  return ret;
	}

  };

#ifdef __cplusplus
}
#endif
#endif
