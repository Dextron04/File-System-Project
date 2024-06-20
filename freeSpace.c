/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha, Hayden Coke
* Student IDs::  922180763, 921741974
* GitHub-Name:: Dextron04, crowcode17
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: freeSpace.c
*
* Description:: This file contains the definitions for functions
* that help in initializing the FAT Free Space System which includes
* initializing FAT, allocating blocks, saving FAT on disk, 
* loading FAT from disk and managing the overall free space of
* disk.
*
**************************************************************/


#include "freeSpace.h"
#include "fileSystem.h"
#include <stdio.h>

int blockFAT; // amount of blocks needed to hold the actual FAT int array
int * FAT;


// FAT table initialization
// takes the total number of blocks in the volume
// returns the amount of blocks used to create the array of integers representing blocks
// note: initializing the FAT does not bring it into memory; the user must load the FAT
// after initializing to modify it
int initializeFAT(int totalBlocks) {
    // blockFAT: Converting total blocks in disk to number of blocks in FAT.
    blockFAT = bytesToBlocks(sizeof(int) * totalBlocks);

    FAT = malloc(blockFAT * BlockSize);    

    // printf("The number of blocks that we need for FAT is: %d\n", blockCountFAT);

    // mark the VCB as the end of its own chain
    FAT[0] = END_OF_BLOB; 

    // chain FAT blocks together
    for(int i = 1; i < totalBlocks; i++) {
        FAT[i] = i + 1;
    }

    // mark end of FAT int array
    FAT[blockFAT - 1] = END_OF_BLOB;

    // mark end of volume
    FAT[totalBlocks - 1] = END_OF_BLOB;

    // the first free block is the block immediately after the FAT
    Vcb->freeChainHead = blockFAT + 1; 

    // save FAT to disk
    saveFAT();

    return Vcb->freeChainHead;
}

// bring FAT into memory
void loadFAT(int numberOfBlocks) {
    blockFAT = bytesToBlocks(sizeof(int) * numberOfBlocks);
    FAT = (int*)malloc(blockFAT * BlockSize);
    LBAread(FAT, blockFAT, 1);
}

// save FAT from memory to disk
// returns -1 if error, 0 if success
int saveFAT() {
    if(FAT == NULL) { 
        return -1;
    }

    LBAwrite(FAT, blockFAT, 1);
    writeVCB();

    return 0;
}

// allocate blocks in the FAT given a requested amount of bytes
// returns the block location of the beginning of the chain
int allocateBlocks(int bytesNumber){
    int returnLocation = Vcb->freeChainHead;

    int blocks = bytesToBlocks(bytesNumber);

    // mark the end of the allocated chain and update the free head
    int chainEnd = findEndOfChain(blocks);

    int nextBlock = FAT[chainEnd]; // block that chainEnd points to

    // the new free chain head is the block after chain end
    Vcb->freeChainHead = nextBlock;

    // mark end of allocated block chain
    FAT[chainEnd] = END_OF_BLOB;

    saveFAT();

    return returnLocation;
}

int deallocateBlocks(int location) {
    int currentBlock = location;
    
    // find the end of the chain starting from the given location
    // currentBlock's value is set to the next one in each iteration.
    while(FAT[currentBlock] != END_OF_BLOB){
        currentBlock = FAT[currentBlock];
    }

    // link the deallocated blocks to the free chain
    FAT[currentBlock] = Vcb->freeChainHead;

    // update free chain head to the start of the deallocated blocks
    Vcb->freeChainHead = location;

    // if deallocating from anywhere else except the start of a file,
    // we need to mark the block right before the given location to EOB
    // to indicate that the file is "shorter". To avoid marking blocks from
    // other files, check that the block at location - 1 is linked to location
    if(FAT[location - 1] == location) {
        FAT[location - 1] = END_OF_BLOB;
    }

    saveFAT();

    return 0;
}



// given a chain length (blocks), finds the end of the chain starting from the FCH
int findEndOfChain(int blocks) {
    int currentBlock = Vcb->freeChainHead;
    for(int i = 0; i < blocks - 1; i++) {
        currentBlock = FAT[currentBlock];
    }

    return currentBlock;
}

int findSentinel(int location) {
    int currentBlock = location;
    currentBlock;
    while(FAT[currentBlock] != END_OF_BLOB){
        currentBlock++;
    }

    return currentBlock;
}

int linkChain(int firstChainTail, int secondChainHead){
    FAT[firstChainTail] = secondChainHead;
    saveFAT();
    
    return 0;
}

int getNextBlock(int currentBlock) {
    return FAT[currentBlock];
}


int freeFAT() {
    free(FAT);
}

void printFAT(int location) {
    while(FAT[location] != END_OF_BLOB){
        printf("FAT[%d] = %d\n", location, FAT[location]);
        location++;
    }   
}

