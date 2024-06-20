/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha
* Student IDs:: 922180763
* GitHub-Name:: Dextron04
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: fs_mkdir.c
*
* Description:: This file implements a function to create a new 
* directory within the file system, handling the allocation of 
* space, and linking it to the parent directory.
*
**************************************************************/

#include "mfs.h"
#include "fileSystem.h"

int fs_mkdir(const char * pathname, mode_t mode){
    ppReturn ret;

    // To calculate the number of entries = Size / sizeof(DirectoryEntry);
    // duplicating the string as it is declared as a constant.
    char * newPath = strdup(pathname);
    int pRet = parsePath(newPath, &ret);


    // If pRet is returning a negative number that means the path
    // that was passed in is invalid and the function will fail 
    // by returning a -1;
    if(pRet != 0){
        return -1;
    }
    
    // If the deNum is not -1 that means the directory that the user is 
    // trying to create already exists and a new directory cannot be created 
    // while the old one with the same name exists.
    if(ret.deNum != -1){
        return -1;
    }
    
    // The createDirectory function is called which takes in the default entry number and
    // the parent which creates a new directory and passes a pointer to its directory entry
    // in the newDir.
    DirectoryEntry * newDir = createDirectory(DEFAULT_ENTRY_NUM, ret.parent);
    if(newDir == NULL){
		printf("[ERROR] Could not create new directory\n");
		return -1;
	}

    // Find empty space in the parent directory to store the newley created directory.
    int freeLocation = findSpaceInDir(ret.parent);

    // Link the newley created directory to the to the parent.
    strcpy(ret.parent[freeLocation].dirName, ret.lastElement);
    ret.parent[freeLocation].location = newDir[0].location;
    ret.parent[freeLocation].size = newDir[0].size;
    ret.parent[freeLocation].isDirectory = newDir[0].isDirectory;
    ret.parent[freeLocation].dateCreated = newDir[0].dateCreated;
    ret.parent[freeLocation].dateModified = newDir[0].dateModified;


    // Finally the save parent containing the new directory.
    saveDir(ret.parent);

    return newDir[0].location;
}