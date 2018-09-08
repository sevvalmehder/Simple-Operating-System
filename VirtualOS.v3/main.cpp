#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"
#include "memory.h"
#include <time.h>

// This is just a sample main function, you should rewrite this file to handle problems
// with new multitasking and virtual memory additions.
int main (int argc, char**argv)
{
	if (argc != 4){
		std::cerr << "Usage: prog exeFile fileSystem debugOption\n";
		exit(1);
	}
	int DEBUG = atoi(argv[3]);
    std::string fileSystemName(argv[2]);

    // Requirement for system call 7 to generate random byte
    srand(time(0));

	memory mem;
	CPU8080 theCPU(&mem);
	GTUOS	theOS(&theCPU, DEBUG, fileSystemName);

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
            if(DEBUG == 1)
                theOS.printContentOfFileTable();
        }



	}	while (!theCPU.isHalted());

    if(!theOS.runningQueue.size() > 1){
        std::cout << "There is a alive thread..." << std::endl;
    }
    // Save the content of the memory
    theOS.writeMemoryIntoFile();

	// Save the content of the file system
    theOS.writeToFile();

    // Writes the directory informatin
    theOS.directoryInformation();

	return 0;
}


