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
* Description:: This file contains a function to retrieve the current 
* working directory's pathname and copy it into a provided buffer.
*
**************************************************************/

#include "mfs.h"
#include "fileSystem.h"

// On success, returns a pointer to a string  
// containing the pathname of the current working directory. 
// On failure, returns NULL 
char * fs_getcwd(char * pathbuffer, size_t size){
	if(size > MAX_PATH_SIZE || size <= strlen(CWDPathname)) {
		return NULL;
	}  

	// copy current working directory into pathBuffer     
	memcpy(pathbuffer, CWDPathname, size);  

	return pathbuffer; 
}