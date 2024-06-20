/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha, Thanh Duong
* Student IDs:: 922180763, 922438176
* GitHub-Name:: Dextron04, DanielDoubleDx
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: fs_rmdir.c
*
* Description:: This file implements a function to remove directories from the file system, 
* ensuring they are empty before deallocating their blocks and updating directory.
*
**************************************************************/

#include "mfs.h"
#include "fileSystem.h"

int fs_rmdir(const char *pathname){
    ppReturn pRet;
    // duplicating the string as it is declared as a constant.
    char * newPath = strdup(pathname);

    int retVal = parsePath(newPath, &pRet);
    free(newPath);

    // will fail if the path is invalid or the directory doesn't exist or the specified 
    // directory entry is not a directory.
    if(retVal == -1 || pRet.deNum == -1 || !pRet.parent[pRet.deNum].isDirectory){
        printf("[ERROR] Could not remove file\n");
        return -1;
    }

    // Loads the directory that is supposed to be removed
    DirectoryEntry * loadedDir = loadDirectory(&pRet.parent[pRet.deNum]);
    if(loadedDir == NULL){
        printf("[ERROR] Could not remove file\n");
        return -1;
    }
    
    // Calculate the number of entries inside the directory.
    // Also save the location and size of the loaded directory for future use.
    int entryCount = (loadedDir->size / sizeof(DirectoryEntry));
    int dirLocation = loadedDir->location;
    int dirSize = loadedDir->size;

    // Will loop through the loaded directory and will skip the '.' (dot) and the '..'
    // (dot dot) directories and will check if the rest of the directory is empty.
    for(int i = 0; i < entryCount; i++){
        if(strcmp(loadedDir[i].dirName, "..") == 0 || strcmp(loadedDir[i].dirName, ".") == 0){
            continue;
        }

        if(strcmp(loadedDir[i].dirName, "") != 0){
            printf("[ERROR] Directory is not empty\n");
            free(loadedDir);
            return -1;
        }
    }

    // If the directory is empty it will deallocate the blocks that were 
    // occupied by the entry.
    deallocateBlocks(loadedDir->location);
    
    // Will change set everything to 0 and null in the parent directory
    // at the appropriate index associated with the directory to be 
    // removed.
    strcpy(pRet.parent[pRet.deNum].dirName, "");
    pRet.parent[pRet.deNum].dateCreated = 0;
    pRet.parent[pRet.deNum].dateModified = 0;
    pRet.parent[pRet.deNum].isDirectory = 0;
    pRet.parent[pRet.deNum].location = 0;
    pRet.parent[pRet.deNum].size = 0;

    // free the memore used by loadedDir.
    // save the parent directory and 
    free(loadedDir);
    saveDir(pRet.parent);

    return 0;
}