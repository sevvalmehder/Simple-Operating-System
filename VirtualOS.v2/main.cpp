#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"
#include "memory.h"
#include <time.h>

// This is just a sample main function, you should rewrite this file to handle problems
// with new multitasking and virtual memory additions.
int main (int argc, char**argv)
{
	if (argc != 3){
		std::cerr << "Usage: prog exeFile debugOption\n";
		exit(1);
	}
	int DEBUG = atoi(argv[2]);

    // Requirement for system call 7 to generate random byte
    srand(time(0));

	memory mem;
	CPU8080 theCPU(&mem);
	GTUOS	theOS(&theCPU, DEBUG);

    // Read the file
	theCPU.ReadFileIntoMemoryAt(argv[1], 0x0000);

    int cycles;

	do
	{
        cycles = theCPU.Emulate8080p(DEBUG);
        theOS.incCycles(cycles);

		if(theCPU.isSystemCall()){
            cycles += theOS.handleCall();
            theOS.incCycles(cycles);
        }



	}	while (!theCPU.isHalted());

    if(!theOS.runningQueue.size() > 1){
        std::cout << "There is a alive thread..." << std::endl;
    }
    // Save the content of the memory
    theOS.writeMemoryIntoFile();

//    std::cout << "çıkışa geldil";
	return 0;
}


