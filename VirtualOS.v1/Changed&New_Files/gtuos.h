#ifndef H_GTUOS
#define H_GTUOS

#include "8080emuCPP.h"

// Define how many cycle these calls
#define CYCLE_PRINT_B 10
#define CYCLE_PRINT_MEM 10
#define CYCLE_READ_B 10
#define CYCLE_READ_MEM 10
#define CYCLE_PRINT_STR 100
#define CYCLE_READ_STR 100
#define CYCLE_GET_RND 5

class GTUOS{
	public:
		uint64_t handleCall(const CPU8080 & cpu);
        void writeMemoryIntoFile(const CPU8080 &cpu);
    private:
		int print_b(const CPU8080 & cpu);
		int print_mem(const CPU8080 &cpu, uint16_t address);
		int read_b(const CPU8080 &cpu);
		int read_mem(const CPU8080 &cpu, uint16_t address);
		int print_str(const CPU8080 &cpu, uint16_t address);
		int read_str(const CPU8080 &cpu, uint16_t address);
		int get_rnd(const CPU8080 &cpu);

};

#endif
