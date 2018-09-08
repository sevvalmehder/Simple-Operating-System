//
// Created by Şevval on 13.06.2018.
//
#include <iostream>

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#define BLOCK_SIZE 256
#define DISK_SPACE 1048576 // 1 MB
#define FILANAME_LIMIT 50

const int NUMBER_OF_BLOCKS = DISK_SPACE / BLOCK_SIZE;

struct file{
    int fileID;
    std::string fileName;
    std::string creationTime;
    std::string lastModificationTime;
    std::string lastAccessTime;
    int fileSize;
    int filePointer;
    bool isOpened;
};

struct block{
    char blockData[BLOCK_SIZE];
    int loadFactor;
    file *data;
};

struct copyBlock{
    char blockData[BLOCK_SIZE];
    int loadFactor;
    int fileID;
    char fileName[50];
    char creationTime[25];
    char lastModificationTime[25];
    char lastAccessTime[25];
    int filePointer;
    bool isOpened;
    int fileSize;
};

class fileSystem {

    friend class GTUOS;

public:
    fileSystem();
    int fileSize;
    void writeTheFileSystemToMemory(std::string filename);
    void readTheFileSystemFromFile(std::string filename);

private:
    void copyTheBlocks();

protected:
    block blocks[NUMBER_OF_BLOCKS];
    copyBlock copyBlocks[NUMBER_OF_BLOCKS];
    void newBlock(int location);


};


#endif //FILESYSTEM_H
