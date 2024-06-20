/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha, Hayden Coke, Eric Khuong, 
*        Thanh Duong
* Student IDs:: 922180763, 921741974, 923406338, 922438176
* GitHub-Name:: Dextron04, crowcode17, ekhuong, DanielDoubleDx
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: DirectoryEntry.h
*
* Description:: The files defines the structure for directory entries.
*
**************************************************************/

#ifndef _DIRECTORY_ENTRY_H
#define _DIRECTORY_ENTRY_H

#include <stdbool.h>

typedef struct DirectoryEntry {
	bool isDirectory;
	char dirName[239];
	unsigned int size;
	unsigned int location;
	unsigned int dateCreated;
	unsigned int dateModified;
} DirectoryEntry;

#endif