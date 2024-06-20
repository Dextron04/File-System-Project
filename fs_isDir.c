/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha
* Student IDs:: 922180763
* GitHub-Name:: Dextron04
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: fs_isDir.c
*
* Description:: This file contains a function to check if 
* a given path corresponds to a directory within the file system.
*
**************************************************************/

#include "mfs.h"
#include "fileSystem.h"

int fs_isDir(char * pathname){
    // duplicating the string as it is declared as a constant.
    char * newPath = strdup(pathname);
    ppReturn ret;

    int retVal = parsePath(newPath, &ret);
    // If the path is invalid then it returns -1.
    if(retVal != 0){
        return -1;
    }

    // If the directory doesn't exist in the parent it will fail.
    // and return -1.
    if(ret.deNum == -1){
        return -1;
    }

    // If the checks fail and everything is right iw will return the
    // boolean value store insdie the directory entry structure named
    // isDir.
    return ret.parent[ret.deNum].isDirectory;
}