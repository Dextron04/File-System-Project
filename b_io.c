/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha, Hayden Coke, Eric Khuong, 
*        Thanh Duong
* Student IDs:: 922180763, 921741974, 923406338, 922438176
* GitHub-Name:: Dextron04, crowcode17, ekhuong, DanielDoubleDx
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: b_io.c
*
* Description:: Basic File System - Key File I/O Operations
*
**************************************************************/

#include "b_io.h"
#include "fileSystem.h"

#define MAXFCBS 20
#define PRE_ALLOC_BLOCKS_INITIAL 50
	
b_fcb fcbArray[MAXFCBS];

int startup = 0;	//Indicates that this has not been initialized

// get current file position
int getFilePosition(b_fcb fcb) {
	return ((B_CHUNK_SIZE * fcb.currentBlock) + fcb.bufferIndex);
}

// get the index of the current block
int getBufferOffset(b_fcb fcb) {
	return (getFilePosition(fcb) % BlockSize);
}

// out of the entire file, return the amount of bytes that
// have not yet been read based on the current file position
int getUnreadBytes(b_fcb fcb) {
	return (fcb.fi->fileSize - getFilePosition(fcb));
}

// creates a new file if one doesn't exist
int createNewFile(ppReturn * pRet, char * newFileName){
	int initialSize = PRE_ALLOC_BLOCKS_INITIAL * BlockSize;
	int index = findSpaceInDir(pRet->parent);
	int location = findInDirectory(newFileName, pRet->parent);
	
	// if the a file with the given name does not exist, location will be -1
	// otherwise, update existing file as if it was new
	if(location > 0){
		index = location; 
	}

	strcpy(pRet->parent[index].dirName, newFileName);
	pRet->parent[index].isDirectory = false;
	pRet->parent[index].dateCreated = time(NULL);
	pRet->parent[index].dateModified = pRet->parent[index].dateCreated;
	pRet->parent[index].location = allocateBlocks(initialSize);
	pRet->parent[index].size = 0;

	if(pRet->parent[index].size < 0){ // calculate valid location later
		printf("[ERROR] Error creating new file\n");
		return -1;
	}

	return index;
}

// Given an amount of blocks, find the location of that block of the file
// example: the file starts at block location 170, and we want the second block
//			of the file, so getBlockLocation may return 171. (files are zero-based)
// works with discontiguously allocated files
int getBlockLocation(b_fcb * fcb, int blocks){
	// start at the block location of the given file
    int currentBlock = fcb->fi->location;

    // Loops until requested block (or end block, if the given block amount was too high)
    for(int i=0; i < blocks; i++){
        currentBlock = getNextBlock(currentBlock);
		if(currentBlock == END_OF_BLOB) {
			return i;
		}
    }

    return currentBlock;
}

//Method to initialize our file system
void b_init () {
	//init fcbArray to all free
	for (int i = 0; i < MAXFCBS; i++)
		{
		fcbArray[i].buffer = NULL; //indicates a free fcbArray
		fcbArray[i].bufferIndex = 0;
		fcbArray[i].fileDescriptor = 0;
		fcbArray[i].currentBlock = 0;
		fcbArray[i].preAllocatedBlocks = 0;
		fcbArray[i].extraBlocks = 0;
		fcbArray[i].isDirty = 0;
		}
		
	startup = 1; // initilization complete
}

//Method to get a free FCB element
b_io_fd b_getFCB () {
	for (int i = 0; i < MAXFCBS; i++)
		{
		if (fcbArray[i].buffer == NULL)
			{
			return i;		//Not thread safe (But do not worry about it for this assignment)
			}
		}
	return (-1);  //all in use
}
	
// Interface to open a buffered file
// flags match the Linux flags for open: O_RDONLY, O_WRONLY, or O_RDWR
b_io_fd b_open (char * filename, int flags){
	if (startup == 0) b_init();  //Initialize our system

	// check if the file can be opened
	b_io_fd returnFd = b_getFCB();		// get our own file descriptor

	// check for error - all used FCB's
	if(returnFd == -1) {
		printf("[ERROR] Open error: max files open\n");
		return -1;
	}
	
	int rwMask = O_RDONLY | O_WRONLY | O_RDWR; // mask for permission bits
	int mask = flags | rwMask; // mask the bits of interest
	int permissionMask = mask & rwMask; // get the bits of interest, with the read/write flags only
	int permission = -1; // read, write, or read/write

	// check read, write, or read and write
	// flags & permissionMask compares the mask of the read/write flags with the given flags,
	// so the result of ANDing the two integers reveals one of the open flags
	if((flags & permissionMask) == O_WRONLY){
		permission = O_WRONLY;
	} else if((flags & permissionMask) == O_RDONLY){
		permission = O_RDONLY;
	} else if((flags & permissionMask) == O_RDWR){
		permission = O_RDWR;
	} else {
		printf("[ERROR] Open error: no read/write permission specified\n");
		return -1;
	}
	
	ppReturn pRet; // store info about the file using the parsePath
	fileInfo * fi; // store file info, such as location, size, and parent
	int fileIndex; // will contain the index of the file inside the parent dir.

	fi = malloc(sizeof(fileInfo));
	if(fi == NULL) {
		printf("[ERROR] MALLOC FAILED\n");
		return -1;
	}

	// check if the given filename was a valid path
	char * newFileName = strdup(filename);
	int retVal = parsePath(newFileName, &pRet);
	if(retVal != 0){
		printf("[ERROR] Open error: %s is not a valid file/path\n", filename);
		return -1;
	}

	// if the user passed in a valid path but the file doesn't exist,
	// create the file if create was specified.
	if(pRet.deNum == -1){
		if(((flags & O_CREAT) == O_CREAT)) {
			// create the file
			// the new index is wherever the new file was created
			fileIndex = createNewFile(&pRet, newFileName); 
			
			if(fileIndex == -1){
				printf("[ERROR] Open error: could not create file\n");
				return -1;
			}

			saveDir(pRet.parent);
		} else {
			printf("[ERROR] Open error: file does not exist\n");
			return -1;
		}
	} else {
		// user can only open files; fail if trying to open a directory
		if(pRet.parent[pRet.deNum].isDirectory) {
			printf("[ERROR] Open error: cannot open a directory\n");
			return -1;
		}

		fileIndex = pRet.deNum;
	}

	// Copy the data from pRet to the fileInfo struct
	strcpy(fi->filename, pRet.parent[fileIndex].dirName);
	fi->fileSize = pRet.parent[fileIndex].size;
	fi->location = pRet.parent[fileIndex].location;
	fi->parent = pRet.parent;
	fi->index = fileIndex;

	// the file buffer is malloc'd here and not freed until the user closes the file
	char * buffer = malloc(B_CHUNK_SIZE); 
	if(buffer == NULL) {
		printf("[ERROR] MALLOC FAILED\n");
		return -1;
	}

	b_fcb fcb = {fi, buffer, 0, false, returnFd, 0, 0, permission, 50, fi->fileSize};
	
	fcbArray[returnFd] = fcb;

	if(((flags & O_CREAT) == O_CREAT)){
		// intitially the file has no size, 
		// but we need to preallocate blocks for if the user
		// writes anything before closing the file
		fcb.preAllocatedBlocks = PRE_ALLOC_BLOCKS_INITIAL;
	} 
	
	if(((flags & O_APPEND) == O_APPEND)){
		// to append to the file, set the file position to the end of the file
		b_seek(returnFd, 0, SEEK_END);
	} 
	
	if(((flags & O_TRUNC) == O_TRUNC)){
		// truncate the file by seeking to the beginning of the file and setting the size to 0
		// when closed, the file will be trimmed
		fcb.fi->fileSize = 0;
		b_seek(returnFd, 0, SEEK_END);
	}

	return returnFd;	// all set
}

// Interface to seek function	
// on success, returns file position
// on fail, returns -1
int b_seek (b_io_fd fd, off_t offset, int whence){
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS)){
		return (-1); 					//invalid file descriptor
	}

	b_fcb * fcb = &fcbArray[fd];

	// what position is the user seeking from?
	int whencePosition = 0;
	switch(whence) {
		case SEEK_CUR: whencePosition = getFilePosition(*fcb);
			break;
		case SEEK_END: whencePosition = fcb->maxFilePos;
			break;
		case SEEK_SET: whencePosition = 0; // start
			break;
		default:
			printf("[ERROR] Seek: invalid whence given\n");
			return -1;
	}

	int newFilePosition = offset + whencePosition;
	int newCurrentBlock = newFilePosition / B_CHUNK_SIZE;

	// check if the new current block is different than the old current block
	// if it is, the buffer is now dirty and needs to be written if it's not dirty already
	// otherwise, update the buffer index
	if(newCurrentBlock != fcb->currentBlock) {
		// when the buffer is dirty the index is effectively irrelevant, so it doesn't get updated
		if(fcb->isDirty) {
			LBAwrite(fcb->buffer, 1, getBlockLocation(fcb, fcb->currentBlock));
		}

		fcb->isDirty = true;
	} else{
		fcb->bufferIndex = (newFilePosition % B_CHUNK_SIZE);

		// edge case: user seeked to a block boundary
		if(offset != 0 && (offset % B_CHUNK_SIZE == 0)) {
			newCurrentBlock++;
		}
	}

	// update the current block
	// do NOT update the max file position, that will get updated if the user does a write
	fcb->currentBlock = newCurrentBlock;
	
	return newFilePosition; 
}


// Interface to write function	
int b_write (b_io_fd fd, char * buffer, int count){
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS)){
		return (-1); 					//invalid file descriptor
	}
	
	// make sure user is trying to write a valid count of bytes
	if(count < 0) return count; 
	
	b_fcb * fcb = &fcbArray[fd];

	if(!((fcb->flag == O_WRONLY) || (fcb->flag == O_RDWR))) {
        printf("[ERROR] Write: File does not have write permissions\n");
        return -1;
    }
	
	// check if the file needs to allocate extra space to accomodate the bytes written,
	// and grow the file as necessary
	checkGrow(fcb, count);

	int bytesWritten = 0;
	int offset = getFilePosition(*fcb) % B_CHUNK_SIZE; 
	// offset is more reliable than buffer index here, since seek may have made the index invalid

	// do a preliminary copy from our buffer to the caller's buffer if possible
	bytesWritten += writePreliminaryBufferCopy(fcb, count, buffer, offset);

	// if there are still bytes remaining to be written, directly write as many bytes as possible
	// from the caller's buffer to the file in full block amounts
	bytesWritten += writeDirectFileCopy(fcb, count - bytesWritten, buffer, bytesWritten);
	
	// by now, the remaining bytes needing to be written should be 0 or less
	// than a full block amount, so we do another buffered write
	bytesWritten += writeSecondaryBufferCopy(fcb, count - bytesWritten, buffer, bytesWritten);

	// after writing, the buffer is no longer valid to read from, and is marked "dirty"
	// if the user called write but passed in a count of 0 (they didn't write anything)
	// then the buffer is not actually dirty, as it hasn't been modified
	if(bytesWritten > 0) {
		fcb->isDirty = true;
	}

	// has the user written further than anything else in the file?
	int filePosition = getFilePosition(*fcb);
	if(filePosition > fcb->maxFilePos){
		fcb->maxFilePos = filePosition;
	}

	return bytesWritten;
}


// Interface to read a buffer

// Filling the callers request is broken into three parts
// Part 1 is what can be filled from the current buffer, which may or may not be enough
// Part 2 is after using what was left in our buffer there is still 1 or more block
//        size chunks needed to fill the callers request.  This represents the number of
//        bytes in multiples of the blocksize.
// Part 3 is a value less than blocksize which is what remains to copy to the callers buffer
//        after fulfilling part 1 and part 2.  This would always be filled from a refill 
//        of our buffer.
//  +-------------+------------------------------------------------+--------+
//  |             |                                                |        |
//  | filled from |  filled direct in multiples of the block size  | filled |
//  | existing    |                                                | from   |
//  | buffer      |                                                |refilled|
//  |             |                                                | buffer |
//  |             |                                                |        |
//  | Part1       |  Part 2                                        | Part3  |
//  +-------------+------------------------------------------------+--------+
int b_read (b_io_fd fd, char * buffer, int count){
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS)) {
		return (-1); 					//invalid file descriptor
	}

	// make sure user is trying to read a valid count of bytes
	if(count < 0) return count; 

	b_fcb * fcb = & fcbArray[fd];

	if(!((fcb->flag == O_RDONLY) || (fcb->flag == O_RDWR))) {
        printf("[ERROR] Read: File does not have read permissions\n");
        return -1;
    }

	int fileUnreadBytes = getUnreadBytes(*fcb);
	// To prevent reading past the file, limit the amount of bytes read
	// to be be equal to the number of unread bytes in the file.
	// So the actual count of how many bytes should be read is whichever is less,
	// the requested count or the number of unread bytes.
	// actual amount of bytes to read
	int byteCount = (count > fileUnreadBytes) ? fileUnreadBytes : count; 

	// track bytes read
	int bytesRead = 0;

	// avoid reading from file if requested bytes are already in file buffer
	bytesRead += readPreliminaryBufferCopy(fcb, byteCount, &buffer);

	// if there are still bytes remaining, check if any blocks can be read directly
	// from the file. blocks can only be copied directly from the file if
	// the bytes remaining are greater than the block size
	bytesRead += readDirectFileCopy(fcb, byteCount - bytesRead, &buffer, bytesRead);

	// if there are still bytes remaining: read to buffer and copy remaining bytes
	bytesRead += readSecondaryBufferCopy(fcb, byteCount - bytesRead, &buffer, bytesRead);

	return bytesRead;
}

	
// Interface to Close the file	
int b_close (b_io_fd fd){
	// check file descriptor
	if ((fd < 0) || (fd >= MAXFCBS)) {
		return -1; 	// invalid file descriptor
	}

	// and check that the specified FCB is actually in use	
	if (fcbArray[fd].fi == NULL) {	
		return -1; // file not open for this descriptor
	}

	b_fcb * fcb = &fcbArray[fd];

	// update modified time to the time the file was closed
	fcb->fi->parent[fcb->fi->index].dateModified = time(NULL);
	saveDir(fcb->fi->parent);

	// write last block if buffer is dirty
	if(fcb->isDirty) {
		char * tempBuffer = malloc(B_CHUNK_SIZE);
		LBAread(tempBuffer, 1, getBlockLocation(fcb, fcb->currentBlock));

		// check- does the buffer actually not match the block?
		if(strcmp(fcb->buffer, tempBuffer) != 0) {
			// clear buffer past index- we don't want to write garbage
			memset(fcb->buffer + fcb->bufferIndex, 0, B_CHUNK_SIZE - fcb->bufferIndex);
			LBAwrite(fcb->buffer, 1, getBlockLocation(&fcbArray[fd], fcb->currentBlock));
		}

		free(tempBuffer);
	}

	// if the maximum file position is zero (empty file) or less than the amount 
	// of blocks we've allocated, trim the excess allocated blocks
	// example: user opens a file, we allocate 50 blocks, and the user closes the file... 
	if((fcb->maxFilePos == 0) || (bytesToBlocks(fcb->maxFilePos) < (bytesToBlocks(fcb->fi->fileSize) + fcb->preAllocatedBlocks))) {
		shrinkSpace(fcb);
	}

	// free buffer and file info memory
	free(fcbArray[fd].buffer);
	free(fcbArray[fd].fi);

	// set this file descriptor as unused
	fcbArray[fd].fi = NULL; 
	fcbArray[fd].buffer = NULL; 
	fcbArray[fd].bufferIndex = 0;
	fcbArray[fd].fileDescriptor = 0;
	fcbArray[fd].currentBlock = 0;
	fcbArray[fd].preAllocatedBlocks = 0;
	fcbArray[fd].currentBlock = 0;
	fcbArray[fd].extraBlocks = 0;
	fcbArray[fd].flag = -1;
	fcbArray[fd].isDirty = 0;

	return 0;

}
