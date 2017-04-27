/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * COPYRIGHT NOTICE. 
 * Copyright (c), 2015 Lierda.
 * All rights reserved.
 * 
 * @file lockqueue.
 * @brief 
 * 
 * @version 1.0
 * @author au
 * @date 2015/05/22-10:25:48
 * @email dongliang@lierda.com
 * 
 * @revision:
 *  - 1.0 2015/05/22 by au.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __LOCKQUEUE_H_
#define __LOCKQUEUE_H_

#include <iostream>
#include <queue>

#include "mutex.h"

namespace util {
    template<typename T> 
        class LockQueue {
    public:
        LockQueue();
        ~LockQueue();
            
        void push(const T &val);
        bool pop(T &val);
        bool pop_back(T &val);
        int size();
    private:
        std::queue<T> elems;
        Mutex mtx;
    };

    template<typename T> inline LockQueue<T>::LockQueue() {
    }
    template<typename T> inline LockQueue<T>::~LockQueue() {
    }
    template<typename T> inline void LockQueue<T>::push(const T &val) {
        mtx.lock();
        elems.push(val);
        mtx.unlock();
    }
    template<typename T> inline bool LockQueue<T>::pop(T &val) {
        bool ret = false;
        mtx.lock();
        if (!elems.empty()) {
            val = elems.front();
            elems.pop();
            ret = true;
        }
        mtx.unlock();
        return ret;
    }
    template<typename T> inline bool LockQueue<T>::pop_back(T &val) {
        bool ret = false;
        mtx.lock();
        if (!elems.empty()) {
            val = elems.front();
            elems.pop();
            ret = true;
        }
        mtx.unlock();
        return ret;
    }
    template<typename T> inline int LockQueue<T>::size() {
        return elems.size();
    }
};
#endif
