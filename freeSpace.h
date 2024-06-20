/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha, Hayden Coke, Eric Khuong, 
*        Thanh Duong
* Student IDs::  922180763, 921741974, 923406338, 922438176
* GitHub-Name:: Dextron04, crowcode17, ekhuong, DanielDoubleDx
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: freeSpace.h
*
* Description:: This file contains all the function prototypes
* and #defines that will be used in the corresponding freespace.c 
* file.
*
**************************************************************/

#ifndef _FREE_SPACE_H
#define _FREE_SPACE_H

#include <stdio.h>

#define END_OF_BLOB 0xFFFFFFFD

// Initalizes the FAT with a new volume given the total number of blocks in the volume
// returns the amount of blocks used for the FAT itself, the array of integers
int initializeFAT(int blockNumber);

// allocates free blocks given a requested amount of bytes
// returns the location of the starting block
int allocateBlocks(int bytesNumber);

int deallocateBlocks(int location);

// brings the FAT into memory from the disk
void loadFAT(int numberOfBlocks);

// saves the FAT to the disk 
// the FAT must be loaded into memory before being written to the disk
// it is the responsibility of the user to save the FAT after making changes
// returns -1 on error, 0 on success
int saveFAT();

// finds the end of a chain of a specified block length starting from the free chain head
// returns the location of the last block
int findEndOfChain(int blocks);
int findSentinel(int location);
int linkChain(int firstChainTail, int secondChainHead);

// frees the memory associated with the FAT array
int freeFAT();

// get next block
int getNextBlock(int currentBlock);

void printFAT(int location);

#endif
