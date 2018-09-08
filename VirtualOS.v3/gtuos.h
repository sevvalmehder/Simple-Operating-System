#ifndef H_GTUOS
#define H_GTUOS

#include <queue>
#include <vector>

#include "8080emuCPP.h"
#include "thread.h"
#include "fileSystem.h"

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
#define QUANTUM_TIME 10000000

class GTUOS{


public:
    GTUOS(CPU8080 *cpu8080, int debugMode, std::string fileSystemName);
    uint64_t handleCall();
    void writeMemoryIntoFile();
    void incCycles(int i);

    virtual ~GTUOS();

    std::queue<thread*> runningQueue;
    std::vector<thread*> threadTable;

    uint64_t numOfCycle;
    int debug;

    void writeToFile();
    void printContentOfFileTable();
    void directoryInformation();



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
    CPU8080 *cpu;
    uint64_t prevNumOfCycle;
    State8080 clear;
    bool isYield;

    fileSystem *OSFileSystem;
    std::vector<file> createdFiles;

    void allboutThreads();

    int fileCreate(uint16_t address);
    int fileClose();
    int fileOpen(uint16_t address);
    int fileRead(uint16_t address);
    int fileWrite(uint16_t address);

    int findFileInBlock(int id);
    int findLocationForFile();
    int findLocationForFile(int fileID);
    int findLocationForFile(std::string filename);

    void readToFile();
    void lastCopyBlocks();
    std::string nameOfFileSystem;


    int dirRead(uint16_t address);

    int fileSeek(uint16_t address);
};


#endif
