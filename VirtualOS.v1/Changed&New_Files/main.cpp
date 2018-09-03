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
	GTUOS	theOS;

    // Initialize with 0
    uint64_t numOfCycle = 0;

    // Read the file
	theCPU.ReadFileIntoMemoryAt(argv[1], 0x0000);	
 
	do	
	{
        numOfCycle += theCPU.Emulate8080p(DEBUG);

        // Wait for an from the keyboard and it will continue for the next tick
        if(DEBUG == 2){
            std::cout << "Press some key to continue: ";
            std::cin.get();
        }

		if(theCPU.isSystemCall())
            numOfCycle += theOS.handleCall(theCPU);

	}	while (!theCPU.isHalted());

    // Save the content of the memory
    theOS.writeMemoryIntoFile(theCPU);
    std::cout << "Total number of cycles used by the program: " << numOfCycle << std::endl;
	return 0;
}

