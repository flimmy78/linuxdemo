/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * COPYRIGHT NOTICE. 
 * Copyright (c), 2015 Lierda.
 * All rights reserved.
 * 
 * @file main.cpp.
 * @brief 
 * 
 * @version 1.0
 * @author au
 * @date 2016/06/22-21:01:23
 * @email dongliang@lierda.com
 * 
 * @revision:
 *  - 1.0 2016/06/22by au.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <iostream>
#include <thread>
#include <list>
#include <sstream>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

#include "SimpleComm.h"
#include "serial.h"


void printbuf(char *buf, int size);

int main(int argc, char *argv[]) {

	util::Serial s("/dev/ttyUSB0", 115200, 8, 'N', 1);
	if (!s.open()) {
		DEBUG_TAG("open serial failed");
		return 0;
	}
	
	while (1) {
		int ret = s.write((char*)"helloworld\r\n", strlen("helloworld\r\n"), 4, 8000);

		if (ret < 0) {
			DEBUG_TAG("write serial failed : " << ret);
			return -1;
		}

		char buf[128];
		ret = s.read(buf, sizeof(buf), 4, 8000);
		if (ret < 0) {
			DEBUG_TAG("read serial failed");
			return -1;
		} else if (ret == 0) {
			continue;
		}
		
		printbuf(buf, ret);

		sleep(1);
	}


  while (1) {
    DEBUG_TAG("this is test file");
    sleep(1000);
  }
  return 0;
}


void printbuf(char *buf, int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		printf("[%02X] ", buf[i]&0xff);
		if ( (i+1) % 20 == 0) {
			printf("\n");
		}
	}
	printf("\n");
}

