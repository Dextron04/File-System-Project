/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha, Hayden Coke, Eric Khuong
* Student IDs:: 922180763, 921741974, 923406338
* GitHub-Name:: Dextron04, crowcode17, ekhuong
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: manageFileSize.c
*
* Description:: The file contains functions to assist in writing files.
* 
*
**************************************************************/

#include "b_io.h"
#include "fileSystem.h"

#define NO_LOCATION -1

// returns how much the file was grown by
int growSpace(b_fcb * fcb, int bytesNeeded){
	// Will determine the minimum number of blocks that will be allocated
	// to accomodate the newley added files
	int filePos = getFilePosition(*fcb);
	int minBlocks = bytesToBlocks(bytesNeeded + filePos);
	int newBlocks = minBlocks;

	// if the minBlocks is greater than the extraBlocks then the
	// the value of the extraBlocks and the newBlocks that will be allocated
	// gets updated.
	if(minBlocks > fcb->extraBlocks){
		fcb->extraBlocks += minBlocks;
		newBlocks += fcb->extraBlocks;
	}
    
	// Allocate new blocks based on the above calculation in full block size
    int newLocation = allocateBlocks(newBlocks * BlockSize);

	// The function will link the end of the old chain and will attach it to the 
	// head of the newley allocated blocks.
	if(fcb->fi->location != NO_LOCATION) {
		int fcbSentinel = findSentinel(fcb->fi->location);
		linkChain(fcbSentinel, newLocation);
	} else {
		fcb->fi->parent[fcb->fi->index].location = newLocation;
		fcb->fi->location = newLocation;
	}

	fcb->fi->fileSize += (newBlocks * BlockSize);

	// save actual DE to reflect changes to the file
	fcb->fi->parent[fcb->fi->index].size = fcb->fi->fileSize;
	saveDir(fcb->fi->parent);

	fcb->preAllocatedBlocks += newBlocks;

	return newBlocks;
}

// This function will check if a file requires a grow.
int checkGrow(b_fcb * fcb, int count){
	int filePos = getFilePosition(*fcb);
	if(filePos + count > fcb->fi->fileSize){
		growSpace(fcb, count);
        return 1; // true if grow is necessary
	}

    return 0; // false if grow is not necessary
}

// Will shrink the space as soon as the file is closed either after
// creation or updation.
int shrinkSpace(b_fcb * fcb){
	int deallocateLocation;

	// it will calculate the exact number of bytes that will be written to the disk
	// and will shrink the space upto the block next to the data location that was 
	// recently added.
	if(fcb->maxFilePos == 0) {
		// deallocate ENTIRE file
		fcb->fi->parent[fcb->fi->index].location = NO_LOCATION;
		deallocateLocation = fcb->fi->location;

	} else {
		fcb->fi->parent[fcb->fi->index].size = fcb->maxFilePos;

		// Convert the maxfilePos to blocks
		int actualBlocksUsed = bytesToBlocks(fcb->maxFilePos);
		int lastBlock = fcb->fi->location;
		
		if(actualBlocksUsed > 1) {
			lastBlock = getBlockLocation(fcb, actualBlocksUsed);
			deallocateLocation = getNextBlock(lastBlock);
		} else {
			saveDir(fcb->fi->parent);
			return 1;
		}
	}

	// deallocate blocks from the calculated new location and save 
	// the updated directory.
	deallocateBlocks(deallocateLocation);
	saveDir(fcb->fi->parent);
	
	// Successful shrink will return 0.
	return 0;
	
}