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
#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
	namespace util {
        enum {
            DEBUG_LEVEL_LOG   = 0x1, //log
            DEBUG_LEVEL_ERROR = 0x2, //error
            DEBUG_LEVEL_PRINT = 0x4, //print
        };
        static int debug_level = DEBUG_LEVEL_LOG | DEBUG_LEVEL_ERROR | DEBUG_LEVEL_PRINT;
		static int debug_has_time = 0;
#define debug_set_level(_level) do {            \
            debug_level = _level;               \
        } while (0)
#define debug_get_level() (error_level)
#define debug_enable_time(_e) (debug_has_time = (_e))
static void debug_print_time(FILE *_file) {
	struct tm tmt;
	time_t tt;
	char time_str[64];
	time(&tt);
	localtime_r(&tt, &tmt);
	sprintf(time_str, "[%02d/%02d/%02d-%02d:%02d:%02d]:", 
			tmt.tm_year+1990, tmt.tm_mon, tmt.tm_mday,
			tmt.tm_hour, tmt.tm_min, tmt.tm_sec);
	fprintf(_file, time_str);
}
#define debug_print(_format, _args...) do {         \
            if (debug_level & DEBUG_LEVEL_PRINT) {  \
				if (debug_has_time) { \
					debug_print_time(stdout); \
				} \
                fprintf(stdout, _format, ##_args);  \
            }                                       \
        } while (0)
#define debug_error(_format, _args...) do {         \
            if (debug_level & DEBUG_LEVEL_ERROR) {  \
				if (debug_has_time) { \
					debug_print_time(stdout); \
				} \
                fprintf(stderr, _format, ##_args);  \
            }                                       \
        } while (0)
#define debug_log(_file, _format, _args...) do {                  \
            if (debug_level & DEBUG_LEVEL_LOG) {                  \
				if (access((_file), F_OK) != 0) break;			  \
                FILE *f = fopen(_file, "a+");                     \
                if (f != NULL) {                                  \
					if (debug_has_time) { \
						debug_print_time(f); \
					} \
                    fprintf(f, _format, ##_args);                 \
                    fclose(f);                                    \
                } else {                                          \
                    fprintf(f, "[#]Cann't Open Log File!\n");     \
                }                                                 \
            }                                                     \
        } while (0)
	};

#ifdef __cplusplus
}
#endif
#endif
