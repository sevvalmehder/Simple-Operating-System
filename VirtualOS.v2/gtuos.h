#ifndef H_GTUOS
#define H_GTUOS

#include <queue>

#include "8080emuCPP.h"
#include "thread.h"

// Define how many cycle these calls
#define CYCLE_PRINT_B 10
#define CYCLE_PRINT_MEM 10
#define CYCLE_READ_B 10
#define CYCLE_READ_MEM 10
#define CYCLE_PRINT_STR 100
#define CYCLE_READ_STR 100
#define CYCLE_GET_RND 5
#define CYCLE_TCREATE 80
#define CYCLE_TEXIT 50
#define CYCLE_TJOIN 40
#define CYCLE_TYIELD 40
#define QUANTUM_TIME 100

class GTUOS{

public:
    GTUOS(CPU8080 *cpu8080, int debugMode);
    uint64_t handleCall();
    void writeMemoryIntoFile();
    void incCycles(int i);

    virtual ~GTUOS();

    std::queue<thread*> runningQueue;
    std::vector<thread*> threadTable;

    uint64_t numOfCycle;
    int debug;


private:
    int print_b();
    int print_mem(uint16_t address);
    int read_b();
    int read_mem(uint16_t address);
    int print_str(uint16_t address);
    int read_str(uint16_t address);
    int get_rnd();
    int tcreate(uint16_t address);
    int tjoin(uint8_t regB);
    int tyield();
    int texit();

    void quantumControl();

    uint16_t lastGivenSp;
    thread *mainThread;
    std::vector<int> waitedIDs;
    CPU8080 *cpu;
    uint64_t prevNumOfCycle;
    State8080 clear;
    bool isYield;

    void allboutThreads();
};

#endif
