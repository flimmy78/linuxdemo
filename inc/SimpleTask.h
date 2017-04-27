/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* COPYRIGHT NOTICE. 
* Copyright (c), 2015 Lierda.
* All rights reserved.
* 
* @file SimpleTask.h.
* @brief 
* 
* @version 1.0
* @author au
* @date 2016/06/25-19:05:15
* @email dongliang@lierda.com
* 
* @revision:
*  - 1.0 2016/06/25by au.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __SIMPLETASK_H_
#define __SIMPLETASK_H_
#include <iostream>
#include <map>
using namespace std;

#include "tcp.h"
#include "serial.h"
#include "lockqueue.h"
#include "cond.h"
using namespace util;

#include <unistd.h>
#include <sys/select.h>

#include "SimpleBuf.h"

#ifdef __cplusplus
extern "C" {
#endif
  typedef int (*FUNC)(SimpleBuf *data);

  class SimpleTask {
  public:
	SimpleTask() {
	  pfd[0] = -1;
	  pfd[1] = -1;
	}
	virtual ~SimpleTask() {
	  pair<int, SimpleBuf*> ct;
	  while (queue.pop(ct)) {
		if (ct.second != NULL) {
		  delete ct.second;
		}
	  }
	}
  public:
	virtual SimpleTask * getClassType() {
	  return this;
	}
	virtual int getRfd() const { 
	  return -1;
	}
	virtual int getWfd() const {
	  return -1; 
	}
	virtual int pushSignal(int _type, char *_data, int _len) {
	  queue.push(pair<int,SimpleBuf*>(_type, new SimpleBuf(_data, _len)));
	  if (pfd[1] > 0) {
		DEBUG_TAG("");
		write(pfd[1], "A", 1);
	  }
	  return 0;
	}
	virtual int recvSignal() {
	  return 0;
	}
	virtual int execTask() { 
	  pair<int,SimpleBuf*> ct;
	  while (queue.pop(ct)) {
		map<int, FUNC>::iterator it = funcs.find(ct.first);
		if (it == funcs.end()) {
		  unsupport(ct.second);
		} else {
		  it->second(ct.second);
		}
		if (ct.second != NULL) {
		  delete ct.second;
		}
	  }
	  if (pfd[0] > 0) {
		int ret;
		fd_set fds;
		do {
		  int ch;
		  FD_ZERO(&fds);
		  FD_SET(pfd[0], &fds);
		  struct timeval tv = {0, 8000};
		  ret  = select(pfd[0] + 1, &fds, NULL, NULL, &tv);
		  if (ret > 0 && FD_ISSET(pfd[0], &fds)) {
			read(pfd[0], &ch, 1);
		  }
		} while (ret > 0 && FD_ISSET(pfd[0], &fds));
	  }
	  return 0; 
	}
	virtual int dead() { 
	  return 0;
	}
	void setWakeDelt(int _delt) {
	  wakeDelt = _delt;
	}
	int getWakeDelt() {
	  return wakeDelt;
	}
	static void viewbuf(char *_buf, int _len) {
	  for (int i = 0; i < _len; i++) {
		printf("[%02X] ", _buf[i]&0xff);
		if ((i+1) % 20 == 0) {
		  printf("\n");
		}
	  }
	  printf("\n");
	}
  private:
	static int unsupport(SimpleBuf *sb) {
	  DEBUG_TAG("");
	  if (sb == NULL) {
		return 0;
	  }
	  viewbuf(sb->getData(), sb->getLen());
	  return 0;
	}
  public:
	map<int, FUNC> funcs;
	LockQueue<pair<int, SimpleBuf*>> queue;
	int pfd[2];
  private:
	int wakeDelt; //ms
  };
#ifdef __cplusplus
}
#endif
#endif
