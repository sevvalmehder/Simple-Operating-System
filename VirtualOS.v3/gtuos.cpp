#include <iostream>
#include <ctime>
#include "8080emuCPP.h"
#include "gtuos.h"

using namespace std;

/*
 * Constructor of GTUOS class
 */
GTUOS::GTUOS(CPU8080 *cpu8080, int debugMode, string fileSystemName) {

    cpu = cpu8080;
    clear = *(cpu->state);

    this->debug = debugMode;
    this->nameOfFileSystem = fileSystemName;

    //
    // THREAD OPERATIONS
    //
    isYield = false;

    // Create a first(main) thread
    mainThread = new thread(0);

    mainThread->registers = clear;

    // Initialize the cycle number with 0
    // then initialize with mainThread cycle number
    numOfCycle = 0;
    prevNumOfCycle = 0;
    mainThread->startingCycle = static_cast<int>(numOfCycle);

    // Stack operation
    mainThread->registers.sp = 0xF000;
    lastGivenSp = 0xF000;

    // Push the main thread to threadTable
    threadTable.push_back(mainThread);

    // Push the main thread to queue
    runningQueue.push(mainThread);

    mainThread->tstate = RUNNING;

    //
    // FILE SYSTEM OPERATIONS
    //

    // Create a file system
    OSFileSystem = new fileSystem();

    // Initialize the file system
    readToFile();


}

uint64_t GTUOS:: handleCall(){


    // The content of register A
    uint8_t contentRegA = cpu->state->a;

    // Address has 16 bit, 8 bit B and 8 bit C
    uint16_t address = (cpu->state->b << 8) | cpu->state->c;

    // Create cycle time variable and initialize with 0
    int cycleTime = 0;

    switch (contentRegA){

        case 1:
            cout << "System Call PRINT_B" << endl;
            cycleTime = print_b();
            cpu->state->pc += 1;
            break;

        case 2:
            cout << "System Call PRINT_MEM" << endl;
            cycleTime = print_mem(address);
            break;

        case 3:
            cout << "System Call READ_B" << endl;
            cycleTime = read_b();
            break;

        case 4:
            cout << "System Call READ_MEM" << endl;
            cycleTime = read_mem(address);
            break;

        case 5:
            cout << "System Call PRINT_STR" << endl;
            cycleTime = print_str(address);
            break;

        case 6:
            cout << "System Call READ_STR" << endl;
            cycleTime = read_str(address);
            break;

        case 7:
            cout << "System Call GET_RND" << endl;
            cycleTime = get_rnd();
            break;

        case 8:
            cout << "System call TExit" << endl;
            cycleTime = texit();
            break;

        case 9:
            cout << "System call TJoin" << endl;
            cycleTime = tjoin(cpu->state->b);
            cpu->state->pc += 1;
            break;

        case 10:
            cout << "System call TYield" << endl;
            cycleTime = tyield();
            break;

        case 11:
            cout << "System call TCreate" << endl;
            cycleTime = tcreate(address);
            cpu->state->pc += 1;
            break;

        case 12:
            cout << "System call FileCreate" << endl;
            cycleTime = fileCreate(address);
            break;

        case 13:
            cout << "System call FileClose" << endl;
            cycleTime = fileClose();
            break;

        case 14:
            cout << "System call FileOpen" << endl;
            cycleTime = fileOpen(address);
            break;

        case 15:
            cout << "System call FileRead" << endl;
            cycleTime = fileRead(address);
            break;

        case 16:
            cout << "System call FileWrite" << endl;
            cycleTime = fileWrite(address);
            break;

        case 17:
            cout << "System call FileSeek" << endl;
            cycleTime = fileSeek(address);
            break;

        case 18:
            cout << "System call DirRead" << endl;
            cycleTime = dirRead(address);
            break;

        default:
            cerr << "Invalid system call" << endl;
            exit(-1);
    }


    return (uint64_t)cycleTime;
}

void GTUOS::incCycles(int cycle) {
    numOfCycle += cycle;
    runningQueue.front()->remainCycle += cycle;

    // And control
    quantumControl();
}

void GTUOS::quantumControl() {

    // If the difference between current and previous number of cycle is greater than quantum time
    // or thread is blocked
    if( ((numOfCycle - prevNumOfCycle) > QUANTUM_TIME)
        || runningQueue.front()->tstate == BLOCKED || isYield ){

        thread* oldT = runningQueue.front();
         oldT->registers = *(cpu->state);
        oldT->startWait = numOfCycle;
        runningQueue.pop();

        if(( debug == 2 || debug == 3 ) && !runningQueue.empty() && oldT->threadID != runningQueue.front()->threadID){
            cout << " Thread switching between " << oldT-> threadID
                 << " , " << runningQueue.front()->threadID << endl;
            if(debug == 2)
                cout << "The total cycles spent for the blocked thread: " << numOfCycle- runningQueue.front()->startWait
                     << endl;
        }


        if(oldT->tstate != BLOCKED){
            // Change the state of old thread as a ready
            oldT->tstate = READY;

            // Add again the back of the queue
            runningQueue.push(oldT);

            // If it is not blocked quantum time is over
            // Change the previous cycle number
            prevNumOfCycle = numOfCycle;
        }
        else
            runningQueue.push(oldT);

        // Pop and push again until the next are not blocked
        while(runningQueue.front()->tstate == BLOCKED){

            thread* temp = runningQueue.front();

            if(temp->waitedThread != -1){
                for( thread* th : threadTable){
                    if(th->threadID == temp->waitedThread
                       && th->isTerminated)
                        runningQueue.front()->tstate = RUNNING;
                }
                if(runningQueue.front()->tstate == BLOCKED){
                    runningQueue.pop();
                    runningQueue.push(temp);
                }
            }
        }

        // Get the unblocked one
        thread* newT = runningQueue.front();
        newT->tstate = RUNNING;

        *(cpu->state) = newT->registers;

        if(debug == 3){
            allboutThreads();
        }


    }

}
/*
 * Prints the contents of register B on the screen as a decimal number.
 */
int GTUOS::print_b(){
    cout << "The content of register B(as a decimal): " << int(cpu->state->b) << endl;

    return CYCLE_PRINT_B;
}

/*
 * Prints the contents of the memory pointed by registers B and C as a decimal number.
 */
int GTUOS::print_mem(uint16_t address){
    cout << "The contents of register BC(as an address): " << address << endl;
    cout << "The content of the memory pointed by this address " << (char)cpu->memory->at(address) << endl;

    return CYCLE_PRINT_MEM;
}

/*
 * Reads an integer from the keyboard and puts it in register B.
 */
int GTUOS::read_b(){

    // Take an user input for register B
    int input;
    cout << "Please enter an integer for register B in bound(0-255): " << endl;
    cin >> input;


    // Check bound error
    if(input >= 0 && input <= 255)
        cpu->state->b = (uint8_t)input;
    else{
        cout << "Out of 8 bit address bound. Assigning zero.. " << endl;
        cpu->state->b = 0;
    }

    cin.clear(); cin.sync();
    return CYCLE_READ_B;
}

/*
 * Reads an integer from the keyboard and puts it in the memory location pointed by registers B and C
 */
int GTUOS::read_mem(uint16_t address){

    // Take an input from user
    int input;
    cout << "Please enter an integer for memory[BC] in bound(0-255): " << endl;
    cin >> input;

    // Check bound error
    if(input >= 0 && input <= 255)
        cpu->memory->at(address) = (uint8_t)input;
    else{
        cout << "Out of 8 bit address bound. Assigning zero.. " << endl;
        cpu->memory->at(address) = 0;
    }

    cin.clear(); cin.sync();
    return CYCLE_READ_MEM;
}

/*
 * Prints the null terminated string at the memory location pointed by B and C
 */
int GTUOS::print_str(uint16_t address){

    for(uint16_t i = address; cpu->memory->at(i) != '\0';  ++i){
        cout << char(cpu->memory->at(i));
    }

    // New line
    cout << endl;

    return CYCLE_PRINT_STR;
}

/*
 * Reads the null terminated string from the keyboard and
 * puts the result at the memory location pointed by B and C
 */
int GTUOS::read_str(uint16_t address){

    // Define input and initialize
    string input;

    cout << "Please give a string: " << endl;
    // Flush to input buffer...
    cin.clear(); cin.sync();

    // Take input from user
    getline(cin, input);
    //cin >> input;

    // Put this input to memory
    uint16_t i = address;
    for(int count = 0; count < input.length(); ++i, ++count){
        cpu->memory->at(i) = static_cast<uint8_t>(input[count]);
    }
    cpu->memory->at(i) = '\0';

    return CYCLE_READ_STR;
}

/*
 * Produces a random byte and puts in register B
 */
int GTUOS::get_rnd(){

    // Create random byte (0-255) and put it on b
    cpu->state->b = static_cast<uint8_t>(rand() % 256);

    return CYCLE_GET_RND;
}

int GTUOS::tcreate(uint16_t address) {

    thread *theNew;

    try{
        // Create a new thread
        theNew = new thread(numOfCycle + CYCLE_TCREATE);
    }
    catch(bad_alloc &exc){
        cpu->state->b = 0;
    }

    theNew->tstate = READY;

    // start address
    theNew->startAddress = address;
    theNew->registers = clear;
    theNew->registers.pc = theNew->startAddress;

    theNew->registers.sp = static_cast<uint16_t>(lastGivenSp - 0x0800);
    lastGivenSp -= 0x0800;

    // Push new thread to threadTable
    threadTable.push_back(theNew);

    // Add the thread to queue
    runningQueue.push(theNew);

    cpu->state->b = static_cast<uint8_t>(theNew->threadID);

    return CYCLE_TCREATE;
}

int GTUOS::tjoin(uint8_t regB) {

    // Register B holds the thread ID of the thread to wait for the content of register B
    runningQueue.front()->waitedThread = regB;

    // Block the calling thread at the current cycle
    runningQueue.front()->tstate = BLOCKED;

    return CYCLE_TJOIN;
}

int GTUOS::tyield() {

    isYield = true;

    runningQueue.front()->registers.pc += 1;
    quantumControl();

    isYield = false;


    return CYCLE_TYIELD;
}

int GTUOS::texit() {

    // Terminate
    runningQueue.front()->isTerminated = true;
    runningQueue.pop();

    *(cpu->state) = runningQueue.front()->registers;

    return CYCLE_TEXIT;
}

void GTUOS::writeMemoryIntoFile(){

    // Open the file
    FILE *f = fopen("exe.mem", "w");

    // Control
    if (f == nullptr)
    {
        printf("error: Couldn't open %s\n", "exe.mem");
        exit(1);
    }

    // Write to file
    // i controls outer loop, initialize i with 0 and increase the i 16 byte
    uint32_t i;
    // j controls inner loop, initialize j with 0 and increase the j 1 byte
    uint16_t j;
    for(i = 0x0; i < 0x10000; i += 0x00010){
        char address[5];
        sprintf(address, "%.4x", i);
        fprintf(f, "%s: ", address);
        for(j = 0; j < 0x00010; ++j){
            fprintf(f, "%.2x ", cpu->memory->at(i+j));
        }
        // Seperate with new line
        fprintf(f, "\n");
    }

    // Close the file
    fclose(f);
}



void GTUOS::allboutThreads() {


    cout << endl <<  "###  INFORMATION ABOUT EACH THREAD  ###" << endl;
    cout << "SITUATION     ID     STATE     STARTED CYCLE     REMAIN CYCLES" << endl;
    for( thread* th: threadTable){
        th->giveInformation();
    }
    cout << endl;
}

int GTUOS::fileCreate(uint16_t address) {

    // Create a file
    file createdFile;

    // Take a name from memory[BC]
    char temp[FILANAME_LIMIT];
    for(int i = 0; i < FILANAME_LIMIT; ++i){
        temp[i] = cpu->memory->at(static_cast<uint32_t>(address + i));
    }
    string filename(temp);

    // Assign the filename
    createdFile.fileID = OSFileSystem->fileSize++;
    createdFile.fileName = filename;
    createdFile.fileSize = 0;

    // Get the current time
    time_t tt = std::time(nullptr);
    string currentTime = std::ctime(&tt);

    createdFile.creationTime = currentTime;
    createdFile.lastAccessTime = "";
    createdFile.lastModificationTime = "";
    createdFile.isOpened = false;
    createdFile.filePointer = -1;

    // Put it on the disk
    int location = findLocationForFile();

    // If location >= 0 successful operation else failure.
    if (location >= 0) {
        createdFiles.push_back(createdFile);
        OSFileSystem->blocks[location].data = &createdFiles.front();
        OSFileSystem->blocks[location].blockData[0] = '\n';
        (cpu->state->b = 1);
    }
    else
        (cpu->state->b = 0);

    return 0;
}

int GTUOS::fileClose() {

    int handleFileID = cpu->state->b;

    int location = findLocationForFile(handleFileID);

    // If location >= 0 the file already opened then close else failure.
    if(location >= 0) {
        createdFiles.at(location).isOpened = false;
        createdFiles.at(location).filePointer = 0;
        (cpu->state->b = 1);
    }
    else
        cpu->state->b = 0;

    return 0;
}

int GTUOS::fileOpen(uint16_t address) {

    // Take a name from memory[BC]
    char temp[FILANAME_LIMIT];
    for(int i = 0; i < FILANAME_LIMIT; ++i){
        temp[i] = cpu->memory->at(static_cast<uint32_t>(address + i));
    }
    string filename(temp);

    int location = findLocationForFile(filename);

    // If location >= 0 open the file and put its handle ID to register B. Else failure.
    if(location >= 0) {
        createdFiles.at(location).isOpened = true;

        // Get the current time
        time_t tt = std::time(nullptr);
        string currentTime = std::ctime(&tt);
        // Update the last access time
        createdFiles.at(location).lastAccessTime = currentTime;

        (cpu->state->b = static_cast<uint8_t>(createdFiles.at(location).fileID));
        createdFiles.at(location).filePointer = 0;
    }
    else{
        cpu->state->b = 0;
        cerr << "File is not created" << endl;
        exit(-1);
    }


    return 0;
}

int GTUOS::fileRead(uint16_t address) {

    int handleID = cpu->state->e;
    int numOfByte = cpu->state->d;

    int location = findFileInBlock(handleID);
    int fileLocation = findLocationForFile(handleID);

    if(OSFileSystem->blocks[location].data != nullptr){
        // Is file opened?
        if(!OSFileSystem->blocks[location].data->isOpened){
            cpu->state->b = 0;
            cerr << "File is not opened" << endl;
            exit(-1);
        }
    }
    else{
        cerr << "File is not created" << endl;
        exit(-1);
    }


    // Where is the file pointer ?
    int startingAddress = OSFileSystem->blocks[location].data->filePointer;

    int i;
    for(i = 0; i < numOfByte; ++i){

        // Read after file pointer location
        char nextData = OSFileSystem->blocks[location].blockData[startingAddress + i];

        // Buffer olarak belirlenen memory[BC]ye bloktaki datayı ata
        if(nextData != '\n'){
            cpu->memory->at(static_cast<uint32_t>(address + i)) = static_cast<uint8_t>(nextData);
        }
        else
            break;
    }

    // Get the current time
    time_t tt = std::time(nullptr);
    string currentTime = std::ctime(&tt);
    // Update the last access time
    createdFiles.at(fileLocation).lastAccessTime = currentTime;

    // Put the readed number of bytes to register B
    cpu->state->b = static_cast<uint8_t>(i);

    // change the file pointer location
    OSFileSystem->blocks[location].data->filePointer += i;
}

int GTUOS::fileWrite(uint16_t address) {

    // Is getting a new block
    bool newBlock = false;
    int handleID = cpu->state->e;
    int numOfByte = cpu->state->d;

    int location = findFileInBlock(handleID);
    int fileLocation = findLocationForFile(handleID);

    // Is file opened?
    if(OSFileSystem->blocks[location].data != nullptr) {
        if (!OSFileSystem->blocks[location].data->isOpened) {
            cpu->state->b = 0;
            cerr << "File is not opened" << endl;
            exit(-1);
        }
    }
    else{
        cerr << "File is not created" << endl;
        exit(-1);
    }

    // Where is the file pointer ?
    int startingAddress = OSFileSystem->blocks[location].data->filePointer;

    int i;
    for(i = 0; i < numOfByte; ++i){

        if(startingAddress + i >= BLOCK_SIZE){
            // Get a new block after the location
            OSFileSystem->newBlock(location);
            newBlock = true;
            break;
        }
        else
            // Buffer olarak belirlenen memory[BC]deki değeri block a ata
            OSFileSystem->blocks[location].blockData[startingAddress + i] = cpu->memory->at(static_cast<uint32_t>(address + i));
    }

    if(newBlock){

        // Write to new block
        for(int j = 0; i < numOfByte; ++i, ++j){
            OSFileSystem->blocks[location + 1].blockData[j]  = cpu->memory->at(static_cast<uint32_t>(address + i));
            OSFileSystem->blocks[location + 1].loadFactor++;
        }
    }
    else{
        // Change the load factor
        OSFileSystem->blocks[location].loadFactor += numOfByte;
    }

    // Get the current time
    time_t tt = std::time(nullptr);
    string currentTime = std::ctime(&tt);
    // Update the last access and modification time
    createdFiles.at(fileLocation).lastAccessTime = currentTime;
    createdFiles.at(fileLocation).lastModificationTime = currentTime;

    // Put the number of bytes writed to register B
    cpu->state->b = static_cast<uint8_t>(i);

    // change the file pointer location
    OSFileSystem->blocks[location].data->filePointer += i;
    OSFileSystem->blocks[location].data->fileSize += i;

    return 0;
}

int GTUOS::fileSeek(uint16_t address) {
    int handleID = cpu->state->d;
    int seekValue = cpu->memory->at(address);

    int location = findFileInBlock(handleID);
    int fileLocation = findLocationForFile(handleID);

    // Is file opened?
    if(OSFileSystem->blocks[location].data != nullptr) {
        if (!OSFileSystem->blocks[location].data->isOpened) {
            cpu->state->b = 0;
            cerr << "File is not opened" << endl;
            exit(-1);
        }
    }
    else{
        cerr << "File is not created" << endl;
        exit(-1);
    }

    if(seekValue > createdFiles.at(fileLocation).fileSize){
        cpu->state->b = 0;
    }
    else{
        createdFiles.at(fileLocation).filePointer = seekValue;
        cpu->state->b = 1;
	// Get the current time
    	time_t tt = std::time(nullptr);
    	string currentTime = std::ctime(&tt);
    	// Update the last access and modification time
    	createdFiles.at(fileLocation).lastAccessTime = currentTime;
    }
	


    return 0;
}

int GTUOS::dirRead(uint16_t address) {

    for(int i = 0; i < NUMBER_OF_BLOCKS; ++i){
        if(OSFileSystem->blocks[i].data != nullptr){
            string input = OSFileSystem->blocks[i].data->fileName;
            for(int count = 0; count < input.length(); ++i, ++count){
                cpu->memory->at(i) = static_cast<uint8_t>(input[count]);
            }
            cpu->memory->at(i) = '\0';
        }
    }
    return 0;
}

int GTUOS::findLocationForFile() {

    for(int i = 0; i < NUMBER_OF_BLOCKS; ++i){
        if(OSFileSystem->blocks[i].data == nullptr)
            return i;
    }
    return -1;
}

int GTUOS::findLocationForFile(int fileID) {

    for(int i = 0; i < createdFiles.size(); ++i){
        // If there is a file in block and the file id is wanted id
        if(createdFiles.at(i).fileID == fileID)
            return i;
    }
    return -1;
}

int GTUOS::findLocationForFile(string filename) {

    for(int i = 0; i < createdFiles.size(); ++i){
        // If there is a file in block and the file id is wanted id
        if(filename == createdFiles.at(i).fileName)
            return i;
    }
    return -1;
}

GTUOS::~GTUOS() {
    delete mainThread;
    delete OSFileSystem;
}

int GTUOS::findFileInBlock(int id) {

    for(int i = 0; i < NUMBER_OF_BLOCKS; ++i){
        if( OSFileSystem->blocks[i].data != nullptr && OSFileSystem->blocks[i].data->fileID == id)
            return i;
    }
    return -1;
}

void GTUOS::writeToFile() {
    OSFileSystem->writeTheFileSystemToMemory(nameOfFileSystem);
}

void GTUOS::lastCopyBlocks() {

    for(int i = 0; i < NUMBER_OF_BLOCKS; ++i){
        if(OSFileSystem->copyBlocks[i].fileName[0] != '\0'){
            file temp;
            temp.creationTime = OSFileSystem->copyBlocks[i].creationTime;
            temp.fileName = OSFileSystem->copyBlocks[i].fileName;
            temp.fileID = OSFileSystem->copyBlocks[i].fileID;
            temp.isOpened = OSFileSystem->copyBlocks[i].isOpened;
            temp.filePointer = OSFileSystem->copyBlocks[i].filePointer;
            temp.lastAccessTime = OSFileSystem->copyBlocks[i].lastAccessTime;
            temp.lastModificationTime = OSFileSystem->copyBlocks[i].lastModificationTime;
            temp.fileSize = OSFileSystem->copyBlocks[i].fileSize;
            createdFiles.push_back(temp);

            strcpy(OSFileSystem->blocks[i].blockData, OSFileSystem->copyBlocks[i].blockData);
            OSFileSystem->blocks[i].loadFactor = OSFileSystem->copyBlocks[i].loadFactor;
            OSFileSystem->blocks[i].data = &createdFiles.front();

        }

    }


}

void GTUOS::readToFile() {
    OSFileSystem->readTheFileSystemFromFile(nameOfFileSystem);
    lastCopyBlocks();

}

void GTUOS::printContentOfFileTable() {
    int i;
    for(i = 0; i < createdFiles.size(); ++i){
        file data = createdFiles.at(i);
        cout << "Filename: " << data.fileName << " file ID: " << data.fileID << " file creation time: " << data.creationTime << " file last modification time: " <<
        data.lastModificationTime << "file last access time: " << data.lastAccessTime << " size of the file: " << data.fileSize << endl;
    }
    if(i == 0)
        cout << "There is no file, so far" << endl;
}

void GTUOS::directoryInformation() {

    int i;

    cout << "Directory has following files: " << endl;
    for(i = 0; i < NUMBER_OF_BLOCKS; ++i){
        if(OSFileSystem->blocks[i].data != nullptr){
            cout << OSFileSystem->blocks[i].data->fileName << " ";
        }
    }
    cout << endl;
}













