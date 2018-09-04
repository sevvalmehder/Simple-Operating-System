#include <iostream>
#include "thread.h"

int thread::count = 0;
thread::thread(uint64_t sCycle) {
    // Give a thread ID
    threadID = count;
    count++;


    startingCycle = sCycle;
    remainCycle = 0;

    // No joined thread at the beginning
    waitedThread = -1;

    // Set a thread state
    tstate = READY;

    isTerminated = false;
}


void thread::giveInformation(){

    std::string situation, state;
    (isTerminated ? situation = "TERMINATED    " : situation = "ALIVE         ");

    switch(tstate){
        case READY: state = "READY  "; break;
        case RUNNING: state = "RUNNING"; break;
        case BLOCKED: state = "BLOCKED"; break;
    }


    std::cout << situation << threadID << "      " << state << "     "
              << startingCycle << "               " << remainCycle << std::endl;

}