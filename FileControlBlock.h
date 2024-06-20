/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha, Hayden Coke, Eric Khuong, 
*        Thanh Duong
* Student IDs:: 922180763, 921741974, 923406338, 922438176
* GitHub-Name:: Dextron04, crowcode17, ekhuong, DanielDoubleDx
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: FileControlBlock.h
*
* Description:: This file defines structures for managing file information 
* and buffering during file operations.
*
**************************************************************/

#ifndef _FILE_CONTROL_BLOCK_H
#define _FILE_CONTROL_BLOCK_H

#include <stdbool.h>
#include "DirectoryEntry.h"

typedef struct fileInfo {
	char filename[239];
	int fileSize;
	int location;
	DirectoryEntry * parent;
	int index;
} fileInfo;

typedef struct b_fcb {
	fileInfo * fi;

	char * buffer;		// holds the open file buffer
	int bufferIndex;	// holds the current position in the buffer
	bool isDirty;		// keeps track of whether or not the buffer is valid
	
	int fileDescriptor;	// "magic number"
	int currentBlock;	// keeps track of which block the file is on
	int preAllocatedBlocks;
	int flag;
	int extraBlocks; // extra block allocation ; just for Eric
	int maxFilePos;

} b_fcb;

#endif