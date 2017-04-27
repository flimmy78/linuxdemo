/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * COPYRIGHT NOTICE. 
 * Copyright (c), 2015 Lierda.
 * All rights reserved.
 * 
 * @file cond.
 * @brief 
 * 
 * @version 1.0
 * @author au
 * @date 2015/05/22-12:35:48
 * @email dongliang@lierda.com
 * 
 * @revision:
 *  - 1.0 2015/05/22 by au.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __COND_H_
#define __COND_H_

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif
    namespace util {
        class Cond {
        public:
            Cond();
            ~Cond();
            void wait();
            void wake();
        private:
            pthread_cond_t cond;
            pthread_mutex_t mutex;
            int cnt;
        };

        Cond::Cond() {
            cnt = 0;
            pthread_cond_init(&cond, NULL);
            pthread_mutex_init(&mutex, NULL);
        }
        Cond::~Cond() {
            pthread_cond_destroy(&cond);
            pthread_mutex_destroy(&mutex);
        }
        inline void Cond::wait() {
            pthread_mutex_lock(&mutex);
            while (cnt == 0) {
                pthread_cond_wait(&cond, &mutex);
            }
            if (cnt > 0) {
                cnt--;
            }
            pthread_mutex_unlock(&mutex);
        }
        inline void Cond::wake() {
            pthread_mutex_lock(&mutex);
            cnt++;
            if (cnt > 0) {
                pthread_cond_signal(&cond);
            }
            pthread_mutex_unlock(&mutex);
        }
    };
#ifdef __cplusplus
}
#endif
#endif
