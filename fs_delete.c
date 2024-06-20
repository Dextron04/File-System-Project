/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha, Thanh Duong
* Student IDs:: 922180763, 922438176
* GitHub-Name:: Dextron04, DanielDoubleDx
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: fs_delete.c
*
* Description:: This file implements a function to delete a file 
* by deallocating its blocks and updating the file system's directory structure.
*
**************************************************************/

#include "mfs.h"
#include "fileSystem.h"

int fs_delete(char* filename){
    ppReturn pRet;
    // duplicating the string as it is declared as a constant.
    char * newFileName = strdup(filename);
    int retVal = parsePath(newFileName, &pRet);

    // will fail if the path is invalid or the directory doesn't exist or the specified 
    // directory entry is not a directory.
    if(retVal == -1){
        printf("[ERROR] could not delete, invalid path\n");
        free(newFileName);

        return -1;
    }

    if(pRet.parent[pRet.deNum].isDirectory){
        printf("[ERROR] could not delete, not a file\n");
        free(newFileName);

        return -1;
    }

    // will deallocate the blocks associated with the file that is
    // supposed to be removed
    deallocateBlocks(pRet.parent[pRet.deNum].location);
    
    // setting everything to null in the directory entry inside the
    // parent so that it doesn't show upwhile reading the driectory.
    strcpy(pRet.parent[pRet.deNum].dirName, "");
    pRet.parent[pRet.deNum].dateCreated = 0;
    pRet.parent[pRet.deNum].dateModified = 0;
    pRet.parent[pRet.deNum].isDirectory = 0;
    pRet.parent[pRet.deNum].location = 0;
    pRet.parent[pRet.deNum].size = 0;
    
    // save the parent directory back to the disk space
    // and writeVCB
    saveDir(pRet.parent);
    writeVCB();

    // Successful removal of the file will return 0.
    return 0;
}