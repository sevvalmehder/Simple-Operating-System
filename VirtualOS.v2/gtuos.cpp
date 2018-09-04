#include <iostream>
#include <algorithm>
#include "8080emuCPP.h"
#include "gtuos.h"

using namespace std;

/*
 * Constructor of GTUOS class
 */
GTUOS::GTUOS(CPU8080 *cpu8080, int debugMode) {

    cpu = cpu8080;
    clear = *(cpu->state);

    this->debug = debugMode;
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
    cout << "The content of the memory pointed by this address" << (int)cpu->memory->at(address) << endl;

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

GTUOS::~GTUOS() {
    delete mainThread;
}

void GTUOS::allboutThreads() {


    cout << endl <<  "###  INFORMATION ABOUT EACH THREAD  ###" << endl;
    cout << "SITUATION     ID     STATE     STARTED CYCLE     REMAIN CYCLES" << endl;
    for( thread* th: threadTable){
        th->giveInformation();
    }
    cout << endl;
}




