/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* COPYRIGHT NOTICE. 
* Copyright (c), 2015 Lierda.
* All rights reserved.
* 
* @file SimpleBuf.h.
* @brief 
* 
* @version 1.0
* @author au
* @date 2016/06/25-19:04:09
* @email dongliang@lierda.com
* 
* @revision:
*  - 1.0 2016/06/25by au.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __SIMPLEBUF_H_
#define __SIMPLEBUF_H_
#include <iostream>
#include <map>
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif
  class SimpleBuf {
  public:
	SimpleBuf(char *_data, int _len) {
	  if (_len <= 0 || _data == NULL) {
		data = NULL;
		len = 0;
		return;
	  }
	  len =_len;
	  data = new char[len];
	  if (data == NULL) {
		len = 0;
		return; 
	  }
	  memcpy(data, _data, len);
	}
	~SimpleBuf() {
	  if (data != NULL) {
		delete[] data;
		data = NULL;
		len = 0;
	  }
	}
	char * getData() const {
	  return data;
	}
	int getLen() const {
	  return len;
	}
  private:
	char *data;
	int len;
  };
#ifdef __cplusplus
}
#endif
#endif
