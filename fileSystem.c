/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha, Hayden Coke, Eric Khuong, 
*        Thanh Duong
* Student IDs:: 922180763, 921741974, 923406338, 922438176
* GitHub-Name:: Dextron04, crowcode17, ekhuong, DanielDoubleDx
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: fileSystem.c
*
* Description:: This file contains the basic initialization of the 
* root directory which is then further facilitated by the the 
* free space manager that provides the free block to allocate the
* root directory in the disk and save it.
*
**************************************************************/

#include "fileSystem.h"

int bytesToBlocks(int bytes)  {
    return ((bytes + BlockSize - 1) / BlockSize);
}

// DE is a pointer to the directory entry that needs to be read from the disk
DirectoryEntry * fillDirectoryBuffer(DirectoryEntry * directoryBuffer, DirectoryEntry * DE) {
    // read one block at a time into buffer 

    // initially, the char buffer and the DE buffer point to the same chunk of memory
    // amount of bytes allocated to the buffer = deSize * BlockSize
    char * buffer = (char *) directoryBuffer; 
    int bufferIndex = 0;
    
    // need to read a total of deSize blocks, or deSize * BlockSize bytes total
    // start reading from the first block 
    int currentBlock = DE->location; 

    // read one block at a time and increment buffer pointer by BlockSize
    while(currentBlock != END_OF_BLOB) {
        LBAread(buffer + bufferIndex, 1, currentBlock);

        // get next block
        currentBlock = getNextBlock(currentBlock);

        // update buffer index
        bufferIndex += BlockSize;
    }

    directoryBuffer = (DirectoryEntry *) buffer;

    return directoryBuffer;
}

DirectoryEntry * loadDirectory(DirectoryEntry * DE){
    if(DE == NULL){
        return NULL;
    }

    int deSize = bytesToBlocks(DE->size);

    // printf("(FS)The DE size is: %d\n", DE->size);
    DirectoryEntry * returnDir = malloc(deSize * BlockSize);

    if(returnDir == NULL) {
        printf("[ERROR] MALLOC FAILED\n");
    }

    fillDirectoryBuffer(returnDir, DE);

    return returnDir;
}

int findSpaceInDir(DirectoryEntry * DE){
	if(DE == NULL){
		return -1;
	}

	for(int i = 0; i < getEntryCount(DE); i++){
		if(DE[i].size == 0 && DE[i].dateCreated == 0){
			return i;
		} 
	}

	return -1;
}

// calculates actual number of directories given an initial amount
// an extra directory is added if there is adequate space
int getTotalEntries(int initialEntryNum){
    int dirSize = sizeof(DirectoryEntry);
    int bytes = initialEntryNum * dirSize;
    int blockAmount = bytesToBlocks(bytes);

    return (blockAmount * BlockSize) / dirSize;
}

int writeVCB(){
    LBAwrite(Vcb, 1, 0);
}

void loadRootDir(){
    LBAread(RootDir, Vcb->rootSize, Vcb->rootLocation);
    if(RootDir == NULL){
        printf("[ERROR] Root Directory was NULL, failed to read from disk\n");
    }
}

void saveDir(DirectoryEntry * DE){
    int blocks = bytesToBlocks(DE[0].size);
    LBAwrite(DE, blocks, DE[0].location);
}

int getEntryCount(DirectoryEntry * DE){
    int entryCount = (DE[0].size / sizeof(DirectoryEntry));
    return entryCount;
}

// create array of directory entries
// if the parent is NULL the created directory is the root directory
DirectoryEntry * createDirectory(int entryNum, DirectoryEntry * parent){
    int entryCount = getTotalEntries(entryNum);
    int direcBytes = entryCount * sizeof(DirectoryEntry);
    int directoryBlocks = BlockSize * bytesToBlocks(direcBytes); // amount of BYTES needed for the blocks
    int directoryLocation = allocateBlocks(direcBytes);
    DirectoryEntry * directory = malloc(directoryBlocks);

    // initialize each directory entry with blank info, indicating the directory is unused
    for(int i = 0; i < entryCount; i++) {
        directory[i].isDirectory = false;
        strcpy(directory[i].dirName, "");
        directory[i].size = 0;
        directory[i].location = 0;
        directory[i].dateCreated = 0;
        directory[i].dateModified = 0;
    }

    // the "dot" entry, representing the "self" directory entry
    directory[0].isDirectory = true;
    strcpy(directory[0].dirName, ".");
    directory[0].size = direcBytes;
    directory[0].location = directoryLocation;
    directory[0].dateCreated =  time(NULL);
    directory[0].dateModified = directory[0].dateCreated;

    // the "dot dot" entry, representing the "parent" directory entry
    // if the given parent was NULL, this directory has no parent,
    // so the root directory is being created
    if(parent == NULL){
        // the root location's "self" and "parent" entries share size, location, and dates
        directory[1].size = directory[0].size;
        directory[1].location = directory[0].location;
        directory[1].dateCreated = directory[0].dateCreated;
        directory[1].dateModified = directory[0].dateModified;

    } else {
        // the parent directory entry gets its size, location, and dateCreated info
        // from the parent, while the dateModified data is the same as the "self" entry
        directory[1].size = parent->size;
        directory[1].location = parent->location;
        directory[1].dateCreated = parent->dateCreated;
        directory[1].dateModified = directory[0].dateModified;

        // update the parent's date modified 
        parent->dateModified = directory[0].dateCreated;
    }

    // regardless of whether or not this is the root directory, the "parent" entry
    // will be named ".." and marked as a directory
    directory[1].isDirectory = true;
    strcpy(directory[1].dirName, "..");

    // save directory to disk
    saveDir(directory);

    return directory;
}
