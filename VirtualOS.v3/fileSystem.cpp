//
// Created by Şevval on 13.06.2018.
//
#include <fstream>
#include <string.h>
#include "fileSystem.h"

fileSystem::fileSystem(){

    // There is not assign file now
    fileSize = 0;

    block temp{};

    for(int i = 0; i < NUMBER_OF_BLOCKS; ++i){
        temp.loadFactor = 0;
        temp.data = nullptr;
        blocks[i] = temp;
    }



}

void fileSystem::newBlock(int location) {

    // Take a new place after the location
    if(blocks[DISK_SPACE-1].data == nullptr){
        // It is empty we can shift the data
        for(int i = DISK_SPACE - 2; i > location; --i){
            blocks[i + 1] = blocks[i];
        }
    }
    else{
        // Disk space is full
        std::cout << "Disk space is full, no more adding" << std::endl;
        exit(-1);
    }

}

void fileSystem::copyTheBlocks() {
    for(int i = 0; i < NUMBER_OF_BLOCKS; ++i){

        if(blocks[i].data != nullptr){
            strcpy (copyBlocks[i].blockData, blocks[i].blockData);
            copyBlocks[i].loadFactor = blocks[i].loadFactor;
            strcpy(copyBlocks[i].fileName, blocks[i].data->fileName.c_str());
            strcpy(copyBlocks[i].lastModificationTime, blocks[i].data->lastModificationTime.c_str());
            strcpy(copyBlocks[i].lastAccessTime, blocks[i].data->lastAccessTime.c_str());
            strcpy(copyBlocks[i].creationTime, blocks[i].data->creationTime.c_str());
            copyBlocks[i].filePointer = blocks[i].data->filePointer;
            copyBlocks[i].isOpened = blocks[i].data->isOpened;
            copyBlocks[i].fileID = blocks[i].data->fileID;
            copyBlocks[i].fileSize = blocks[i].data->fileSize;
        }

    }
}

void fileSystem::writeTheFileSystemToMemory(std::string filename){

    copyTheBlocks();
    std::ofstream output_file(filename, std::ios::binary);
    output_file.write((char*)copyBlocks, sizeof(copyBlocks));
    output_file.close();
}

void fileSystem::readTheFileSystemFromFile(std::string filename) {

    std::ifstream input_file(filename, std::ios::binary);
    input_file.read((char*)copyBlocks, sizeof(copyBlocks));
    input_file.close();

}