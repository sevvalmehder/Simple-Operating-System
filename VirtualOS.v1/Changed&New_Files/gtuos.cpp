#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"

uint64_t GTUOS::handleCall(const CPU8080 & cpu){


    // The content of register A
    uint8_t contentRegA = cpu.state->a;

    // Address has 16 bit, 8 bit B and 8 bit C
    uint16_t address = (cpu.state->b << 8) | cpu.state->c;

    // Create cycle time variable and initialize with 0
    int cycleTime = 0;

    switch (contentRegA){

        case 1:
            std::cout << "System Call PRINT_B" << std::endl;
            cycleTime = print_b(cpu);
            break;

        case 2:
            std::cout << "System Call PRINT_MEM" << std::endl;
            cycleTime = print_mem(cpu, address);
            break;

        case 3:
            std::cout << "System Call READ_B" << std::endl;
            cycleTime = read_b(cpu);
            break;

        case 4:
            std::cout << "System Call READ_MEM" << std::endl;
            cycleTime = read_mem(cpu, address);
            break;

        case 5:
            std::cout << "System Call PRINT_STR" << std::endl;
            cycleTime = print_str(cpu, address);
            break;

        case 6:
            std::cout << "System Call READ_STR" << std::endl;
            cycleTime = read_str(cpu, address);
            break;

        case 7:
            std::cout << "System Call GET_RND" << std::endl;
            cycleTime = get_rnd(cpu);
            break;

    }

    return (uint64_t)cycleTime;
}

/*
 * Prints the contents of register B on the screen as a decimal number.
 */
int GTUOS::print_b(const CPU8080 &cpu){
    std::cout << "The content of register B(as a decimal): " << int(cpu.state->b) << std::endl;

    return CYCLE_PRINT_B;
}

/*
 * Prints the contents of the memory pointed by registers B and C as a decimal number.
 */
int GTUOS::print_mem(const CPU8080 &cpu, uint16_t address){
    std::cout << "The contents of register BC(as an address): " << address << std::endl;
    std::cout << "The content of the memory pointed by this address" << (int)cpu.memory->at(address) << std::endl;

    return CYCLE_PRINT_MEM;
}

/*
 * Reads an integer from the keyboard and puts it in register B.
 */
int GTUOS::read_b(const CPU8080 &cpu){

    // Take an user input for register B
    int input;
    std::cout << "Please enter an integer for register B in bound(0-255): " << std::endl;
    std::cin >> input;

    // Check bound error
    if(input >= 0 && input <= 255)
        cpu.state->b = (uint8_t)input;
    else{
        std::cout << "Out of 8 bit address bound. Assigning zero.. " << std::endl;
        cpu.state->b = 0;
    }

    return CYCLE_READ_B;
}

/*
 * Reads an integer from the keyboard and puts it in the memory location pointed by registers B and C
 */
int GTUOS::read_mem(const CPU8080 &cpu, uint16_t address){

    // Take an input from user
    int input;
    std::cout << "Please enter an integer for memory[BC] in bound(0-255): " << std::endl;
    std::cin >> input;

    // Check bound error
    if(input >= 0 && input <= 255)
        cpu.memory->at(address) = (uint8_t)input;
    else{
        std::cout << "Out of 8 bit address bound. Assigning zero.. " << std::endl;
        cpu.memory->at(address) = 0;
    }

    return CYCLE_READ_MEM;
}

/*
 * Prints the null terminated string at the memory location pointed by B and C
 */
int GTUOS::print_str(const CPU8080 &cpu, uint16_t address){

    for(uint16_t i = address; cpu.memory->at(i) != '\0';  ++i){
        std::cout << char(cpu.memory->at(i));
    }

    // New line
    std::cout << std::endl;

    return CYCLE_PRINT_STR;
}

/*
 * Reads the null terminated string from the keyboard and
 * puts the result at the memory location pointed by B and C
 */
int GTUOS::read_str(const CPU8080 &cpu, uint16_t address){

    // Define input and initialize
    std::string input = "";

    std::cout << "Please give a string: " << std::endl;
    // Flush to input buffer...
    std::cin.clear();
    std::cin.sync();

    // Take input from user
    getline(std::cin, input);
    //std::cin >> input;

    // Put this input to memory
    uint16_t i = address;
    for(int count = 0; count < input.length(); ++i, ++count){
        cpu.memory->at(i) = input[count];
    }
    cpu.memory->at(i) = '\0';

    return CYCLE_READ_STR;
}

/*
 * Produces a random byte and puts in register B
 */
int GTUOS::get_rnd(const CPU8080 &cpu){

    // Create random byte (0-255) and put it on b
    cpu.state->b = rand() % 256;

    return CYCLE_GET_RND;
}

void GTUOS::writeMemoryIntoFile(const CPU8080 &cpu){

    // Open the file
    FILE *f = fopen("exe.mem", "w");

    // Control
    if (f == NULL)
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
            fprintf(f, "%.2x ", cpu.memory->at(i+j));
        }
        // Seperate with new line
        fprintf(f, "\n");
    }

    // Close the file
    fclose(f);
}
