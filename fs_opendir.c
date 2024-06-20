/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha
* Student IDs:: 922180763
* GitHub-Name:: Dextron04
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: fs_opendir.c
*
* Description:: This file implements a function to open directories.
*
**************************************************************/

#include "mfs.h"
#include "fileSystem.h"

fdDir * fs_opendir(const char *pathname){
    ppReturn ret;
    fdDir * fd;
    // malloc size for the file descriptor.
    fd = malloc(sizeof(fdDir));

    // duplicating the string as it is declared as a constant.
    char * newPath = strdup(pathname);
    int pRet;
    pRet = parsePath(newPath, &ret);
    
    // If the directory doesn't exist deNum will be -1 and the function will fail.
    if(ret.deNum == -1){
        return NULL;
    }

    DirectoryEntry * returnDir;

    // If the returned value is -2 that signifies Root Directory
    // and becasue of that we load the root directory and if not then
    // we load the directory associated with the deNum.
    if(pRet == -2){
        returnDir = RootDir;
    } else{
        returnDir = loadDirectory(&ret.parent[ret.deNum]);
    }

    // Putting all the relavent data in the fdDir struct to keep the file open.
    fd->d_reclen = sizeof(fdDir);
    fd->dirEntryPosition = 0; // The entry position of the file in the dir.
    fd->directory = returnDir; // pointer to the parent.
    fd->entryCount = getEntryCount(returnDir); // Gets the entry count for the loaded directory
    fd->di = malloc(fd->entryCount * sizeof(struct fs_diriteminfo)); 

    // Returning the file descriptor.
    return fd;

}