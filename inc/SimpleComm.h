/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* COPYRIGHT NOTICE. 
* Copyright (c), 2015 Lierda.
* All rights reserved.
* 
* @file SimpleComm.h.
* @brief 
* 
* @version 1.0
* @author au
* @date 2016/06/25-19:15:11
* @email dongliang@lierda.com
* 
* @revision:
*  - 1.0 2016/06/25by au.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __SIMPLECOMM_H_
#define __SIMPLECOMM_H_
#ifdef __cplusplus
extern "C" {
#endif

#ifndef uint8 
#define uint8 unsigned char
#endif
#ifndef uint8_t
#define uint8_t unsigned char
#endif
#ifndef int8 
#define int8 char
#endif
#ifndef int8_t
#define int8_t char
#endif
#ifndef uint16
#define uint16 unsigned short
#endif
#ifndef uint16_t
#define uint16_t unsigned short
#endif
#ifndef int16 
#define int16 short
#endif
#ifndef int16_t
#define int16_t short
#endif
#ifndef uint32 
#define uint32 unsigned int
#endif
#ifndef uint32_t
#define int32_t unsigned int
#endif
#ifndef int32 
#define int32 int
#endif
#ifndef int32_t
#define int32_t int
#endif
#ifndef uint64
#define uint64 unsigned long long
#endif
#ifndef uint64_t
#define uint64_t unsigned long long
#endif
#ifndef int64 
#define int64 long long
#endif
#ifndef int64_t
#define int64_t long long
#endif

#define DEBUG_TAG(x) do {cout << "<" << __FILE__ << "@" << dec << __LINE__ << "$" << __func__ << ">:" << x << endl;} while (0)

#define HI_UINT16(a) ((uint8)(((a) >> 8) & 0xFF))
#define LO_UINT16(a) ((uint8)(((a) & 0xFF)))
#define BUILD_UINT16(loByte, hiByte)  ((uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#ifdef __cplusplus
}
#endif
#endif
