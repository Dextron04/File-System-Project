/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha, Hayden Coke, Eric Khuong, 
*        Thanh Duong
* Student IDs:: 921741974
* GitHub-Name:: crowcode17
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: b_writeHelpers.c
*
* Description:: The file contains functions to assist in writing files.
* 
*
**************************************************************/

#include "b_io.h"
#include "fileSystem.h"

int writePreliminaryBufferCopy(b_fcb * fcb, int byteCount, char * buffer, int offset){
	// read the block, add to it, and write it back
	// read the current block
	LBAread(fcb->buffer, 1, getBlockLocation(fcb, fcb->currentBlock));

	// copy from caller's buffer to our buffer, using offset as buffer index
	int bytesRemaining = (B_CHUNK_SIZE - offset);
	int copyCount; 
	// int copyCount = (byteCount > bytesRemaining) ? bytesRemaining : byteCount;
	if(byteCount > bytesRemaining){
		copyCount = bytesRemaining;
	}else{
		copyCount = byteCount;
	}

	// if the user wants to write MORE than the space left in the buffer,
	// ONLY copy to fill the buffer
	// if the user wants to write LESS than or equal to the amount of space left,
	// only copy their amount
	memcpy(fcb->buffer + fcb->bufferIndex, buffer, copyCount);	

	// write buffer to file
	LBAwrite(fcb->buffer, 1, getBlockLocation(fcb, fcb->currentBlock));

	// update buffer index
	fcb->bufferIndex = offset + copyCount;
	
	if(fcb->bufferIndex == B_CHUNK_SIZE) {
		fcb->bufferIndex = 0;
		fcb->currentBlock += 1;
	}

	return copyCount;
}

// writes from caller buffer directly to file
// returns bytes read
// buffer parameter is the caller buffer
int writeDirectFileCopy(b_fcb * fcb, int byteCount, char * buffer, int callIndex){
	if(byteCount == 0) return 0;

	int blocks = bytesToBlocks(byteCount);

    if(byteCount % BlockSize != 0) {
        blocks = blocks - 1;
    }

	LBAwrite(buffer + callIndex, blocks, getBlockLocation(fcb, fcb->currentBlock));

	fcb->currentBlock += blocks;

	int bytesWritten = B_CHUNK_SIZE * blocks;
	
	return bytesWritten;
}

// write from file to file buffer, then copies bytes to caller buffer
// returns bytes read
int writeSecondaryBufferCopy(b_fcb * fcb, int byteCount, char * buffer, int callIndex){
	if(byteCount == 0) return 0;

	// Copy from the caller buffer to the buffer
	memcpy(fcb->buffer + fcb->bufferIndex, buffer + callIndex, byteCount);

	// Write the number of bytes from the buffer to the file system
	LBAwrite(fcb->buffer, 1, getBlockLocation(fcb, fcb->currentBlock));

	// Update the buffer index
	fcb->bufferIndex += byteCount;

	// printf("(BWR - sec) buffer index: %d", fcb->bufferIndex);

	if(fcb->bufferIndex == B_CHUNK_SIZE) {
		fcb->bufferIndex = 0;
		fcb->currentBlock += 1;
	}
	
	return byteCount;
}
