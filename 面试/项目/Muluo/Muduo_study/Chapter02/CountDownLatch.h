//
// Created by jxq on 19-8-19.
//

// 倒计时

#ifndef MYMUDUO_COUNTDOWNLATCH_H
#define MYMUDUO_COUNTDOWNLATCH_H

#include "Mutex.h"
#include "Condition.h"

class CountDownLatch
{
public:
    CountDownLatch(int count)
        : mutex_(),
          cond_(mutex_),    // 初始化顺序要和成员声明保持一致
          count_(count)
    {

    }

private:
    mutable MutexLock mutex_;   // 顺序很重要，先mutex后condition;
    Condition cond_;
    int count_;
};

#endif //MYMUDUO_COUNTDOWNLATCH_H
