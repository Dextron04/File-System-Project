/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha, Hayden Coke, Eric Khuong, 
*        Thanh Duong
* Student IDs::  922180763, 921741974, 923406338, 922438176
* GitHub-Name:: Dextron04, crowcode17, ekhuong, DanielDoubleDx
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: fileSystem.h
*
* Description:: This file contains all the function prototypes
* and the #defines that are necessary for the working of the 
* fileSystem.c file.
*
**************************************************************/

#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "fsLow.h"
#include "mfs.h" // mfs already contains the DE struct.
#include "freeSpace.h"

// Directory Entry structure
// this structure contains essential directory entry info,
// including timestamp metadata

#define MAX_PATH_SIZE 4096

extern int BlockSize;
extern DirectoryEntry * RootDir;
extern DirectoryEntry * CurrentWorkingDir;
extern char * CWDPathname;

// Volume Control Block structure 
// contains essential information for formatting the volume
// this structure is initialized when the file system is initialized
typedef struct VCB{
	unsigned long long int signature;
	unsigned int blockCount;
	unsigned int rootLocation;
	unsigned int rootSize;
	unsigned int freeSpaceLocation;
	unsigned int freeChainHead;
}VCB;


typedef struct ppReturn{
	DirectoryEntry * parent;
	unsigned int deNum;
	char * lastElement;
} ppReturn;

extern VCB * Vcb;

// formula for bytes to total blocks
int bytesToBlocks(int bytes);

int findInDirectory(char * name, DirectoryEntry * parent);
int getEntryCount(DirectoryEntry * DE);
int findSpaceInDir(DirectoryEntry * DE);
int createNewFile(ppReturn * pRet, char * newFileName);

// creates a directory given a requested amount of entries and a parent directory entry
// returns an array of directory entries as a pointer to the first directory entry
// if the given parent is NULL, the root directory is being created
DirectoryEntry * createDirectory(int entries, DirectoryEntry * parent);

int parsePath(char * pathname, ppReturn * ret);
DirectoryEntry * loadDirectory(DirectoryEntry * DE);
void loadRootDir();
void saveDir(DirectoryEntry * DE);

// Writes the VCB to the first block in the disk space
int writeVCB();

#endif


