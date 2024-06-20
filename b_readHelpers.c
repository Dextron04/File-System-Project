/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Hayden Coke
* Student IDs:: 921741974
* GitHub-Name:: crowcode17
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: b_readHelpers.c
*
* Description:: The file contains functions to assist in reading files, 
* including preliminary buffer copying, direct file copying, and secondary 
* buffer copying to manage data retrieval from disk to memory buffers.
*
**************************************************************/

#include "b_io.h"
#include "fileSystem.h"

// if the file buffer is empty (first time reading file), returns 0
// otherwise returns bytes read
int readPreliminaryBufferCopy(b_fcb * fcb, int byteCount, char ** buffer){
	// the buffer is only valid to read from if it is not empty or dirty
	if(fcb->bufferIndex == 0 || fcb->isDirty) {
		return 0;
	}

	// how many bytes are left unread in the buffer?
	int bytesRemaining = B_CHUNK_SIZE - fcb->bufferIndex;

	// do not copy past the buffer
	int copyCount = (byteCount > bytesRemaining) ? bytesRemaining : byteCount;

	memcpy(*buffer, fcb->buffer + fcb->bufferIndex, copyCount);

	// update index and block if necessary
	fcb->bufferIndex += copyCount;

	// buffer index is at end of block
	if(fcb->bufferIndex == B_CHUNK_SIZE) {
		fcb->bufferIndex = 0;
		fcb->currentBlock += 1;
	}

	return copyCount;
}

// reads from file directly to caller buffer
// returns bytes read
// buffer parameter is the caller buffer
int readDirectFileCopy(b_fcb * fcb, int byteCount, char ** buffer, int callIndex){
	if(byteCount == 0) return 0;

	int blocks = bytesToBlocks(byteCount);

	if(byteCount % BlockSize != 0) {
        blocks = blocks - 1;
    }

	LBAread(*buffer + callIndex, blocks, getBlockLocation(fcb, fcb->currentBlock));

	fcb->currentBlock += blocks;

	int bytesRead = B_CHUNK_SIZE * (blocks);

	return bytesRead;
}

// reads from file to file buffer, then copies bytes to caller buffer
// returns bytes read
int readSecondaryBufferCopy(b_fcb * fcb, int byteCount, char ** buffer, int callIndex){
	if(byteCount == 0) return 0;

	LBAread(fcb->buffer, 1, getBlockLocation(fcb, fcb->currentBlock));

	memcpy(*buffer + callIndex, fcb->buffer + fcb->bufferIndex, byteCount);

	fcb->bufferIndex += byteCount;

	return byteCount;
}
