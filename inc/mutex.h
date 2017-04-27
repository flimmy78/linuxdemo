/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * COPYRIGHT NOTICE. 
 * Copyright (c), 2015 Lierda.
 * All rights reserved.
 * 
 * @file mutex.
 * @brief 
 * 
 * @version 1.0
 * @author au
 * @date 2015/05/22-12:29:01
 * @email dongliang@lierda.com
 * 
 * @revision:
 *  - 1.0 2015/05/22 by au.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __MUTEX_H_
#define __MUTEX_H_

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

    namespace util {
        class Mutex {
        public:
            Mutex();
            ~Mutex();
            
            void lock();
            void unlock();

        private:
            pthread_mutex_t mutex;
        };

        Mutex::Mutex() {
            pthread_mutex_init(&mutex, NULL);
        }
        Mutex::~Mutex() {
            pthread_mutex_destroy(&mutex);
        }
        inline void Mutex::lock() {
            pthread_mutex_lock(&mutex);
        }
        inline void Mutex::unlock() {
            pthread_mutex_unlock(&mutex);
        }
    };

#ifdef __cplusplus
}
#endif

#endif
