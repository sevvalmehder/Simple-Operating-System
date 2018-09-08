//
// Created by Şevval on 16.04.2018.
//

#ifndef THREAD_H
#define THREAD_H

#include "8080emuCPP.h"

enum threadState{
    READY = 0,
    RUNNING = 1,
    BLOCKED = -1,
} ;

class thread{

public:
    explicit thread(uint64_t sCycle);
    State8080 registers;
    int threadID;
    threadState tstate;
    uint16_t startAddress;
    int avaiableSpace;

    bool isTerminated;
    uint64_t startWait;
    uint64_t startingCycle;
    uint64_t remainCycle;

    // If thread is joined
    int waitedThread;


    void giveInformation();

private:
    static int count;


};



#endif //THREAD_H

