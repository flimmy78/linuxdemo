/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * COPYRIGHT NOTICE. 
 * Copyright (c), 2015 Lierda.
 * All rights reserved.
 * 
 * @file debug.
 * @brief 
 * 
 * @version 1.0
 * @author au
 * @date 2015/06/15-12:54:22
 * @email dongliang@lierda.com
 * 
 * @revision:
 *  - 1.0 2015/06/15 by au.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __DEBUGX_H_
#define __DEBUGX_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif


#define RESET   			"\033[0m"
#define BLACK   			"\033[30m"      /* Black */
#define RED     			"\033[31m"      /* Red */
#define GREEN   			"\033[32m"      /* Green */
#define YELLOW  			"\033[33m"      /* Yellow */
#define BLUE    			"\033[34m"      /* Blue */
#define MAGENTA 			"\033[35m"      /* Magenta */
#define CYAN    			"\033[36m"      /* Cyan */
#define WHITE   			"\033[37m"      /* White */
#define BOLDBLACK   	"\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     	"\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   	"\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  	"\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    	"\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA 	"\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    	"\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   	"\033[1m\033[37m"      /* Bold White */

static void dget_curttime(char *buf) {
	struct tm tmt;
	time_t tt;
	time(&tt);
	localtime_r(&tt, &tmt);
	sprintf(buf, 
					"%02d/%02d/%02d-%02d:%02d:%02d", 
					tmt.tm_year+1990, tmt.tm_mon, tmt.tm_mday,
					tmt.tm_hour, tmt.tm_min, tmt.tm_sec);
}
static void dprint(char *_file, char *mode, char *lvl, char *val) {
	char curtime[64];
	char buf[1024];
	int size = 0;
	dget_curttime(curtime);
	char *color = NULL;
	size =size;
	if (strcmp(mode, "EROR") == 0) {
		color = (char*)RED;
	} else if (strcmp(mode, "WARN") == 0) {
		color = (char*)YELLOW;
	} else {
		color = (char*)RESET;
	}

	size = sprintf(buf, "%s[%s][%s][%s]%s:%s", color, mode, lvl, curtime, RESET, val);

	if (_file == NULL) {
#ifdef __cplusplus
	cout << buf << endl;
#else
	fprintf(stdout, "%s\n", buf);
#endif
	} else {
		if (access((_file), F_OK) != 0) {
			return;
		}
		FILE *f = fopen(_file, "a+"); 
		if (f != NULL) {                                  
			fprintf(f, "%s\n", buf);
			fclose(f);
		} else {
			// can't open the log file, may log out to syslog ?
			// now not deal it.
			;
		}
	}
}

static void dprint_buf(char *_file, char *mode, char *lvl, char *b, int s) {
	char curtime[64];
	char buf[1024];
	int i = 0;
	int size = 0;
	char *color = NULL;
	dget_curttime(curtime);
	
	if (strcmp(mode, "EROR") == 0) {
		color = (char*)RED;
	} else if (strcmp(mode, "WARN") == 0) {
		color = (char*)YELLOW;
	} else {
		color = (char*)RESET;
	}

	size = sprintf(buf, "%s[%s][%s][%s]%s:", color, mode, lvl, curtime, RESET);
	for (i = 0; i < s; i++) {
		if (i % 20 == 0) {
			size += sprintf(buf + size, "\n");
		}
		size += sprintf(buf + size, "[%02X] ", b[i]&0xff);
	}

	if (_file == NULL) {
#ifdef __cplusplus
	cout << buf << endl;
#else
	fprintf(stdout, "%s\n", buf);
#endif
	} else {
		if (access((_file), F_OK) != 0) {
			return;
		}
		FILE *f = fopen(_file, "a+"); 
		if (f != NULL) {                                  
			fprintf(f, "%s\n", buf);
			fclose(f);
		} else {
			// can't open the log file, may log out to syslog ?
			// now not deal it.
			;
		}
	}
}

#define ENALBE_DEBUG 1
	static char *__log_file__ 	= NULL;
#if ENALBE_DEBUG
	#define DEBUG_INFO(x)						dprint(NULL, (char*)"DEBUG", (char*)"INFO", (char*)(x))
	#define DEBUG_WARN(x)						dprint(NULL, (char*)"DEBUG", (char*)"WARN", (char*)(x))
	#define DEBUG_ERROR(x) 					dprint(NULL, (char*)"DEBUG", (char*)"EROR", (char*)(x))
	#define DEBUG_INFO_BUF(b, s) 		dprint_buf(NULL, (char*)"DEBUG", (char*)"INFO", (char*)(b), (int)(s))
	#define LOGGG_INFO(x)						dprint(__log_file__, (char*)"LOG", (char*)"INFO", (char*)(x))
	#define LOGGG_WARN(x)						dprint(__log_file__, (char*)"LOG", (char*)"WARN", (char*)(x))
	#define LOGGG_ERROR(x)					dprint(__log_file__, (char*)"LOG", (char*)"EROR", (char*)(x))
	#define LOGGG_INFO_BUF(b, s)		dprint_buf(__log_file__, (char*)"LOG", (char*)"INFO", (char*)(b), (int)(s)) 
	#define DEBUG_LOG_INIT(x)				do { \
																				__log_file__ = ((char*)(x)); \
																			 	DEBUG_INFO("debug enabled"); \
																				DEBUG_INFO_BUF("AAA", 3); \
																	} while (0) 
#else
	#define DEBUG_INFO(x)					
	#define DEBUG_WARN(x) 	
	#define DEBUG_ERROR(x)	
	#define DEBUG_INFO_BUF(b, s) 
	#define LOGGG_INFO(x)
	#define LOGGG_WARN(x)
	#define LOGGG_ERROR(x)
	#define LOGGG_INFO_BUF(b, s)
	#define DEBUG_LOG_INIT				
#endif
	
#ifdef __cplusplus
}
#endif
#endif
