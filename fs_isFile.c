/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha
* Student IDs:: 922180763
* GitHub-Name:: Dextron04
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: fs_isFile.c
*
* Description:: This file contains a function to determine if 
* a given path corresponds to a file within the file system.
*
**************************************************************/

#include "mfs.h"
#include "fileSystem.h"

int fs_isFile(char * filename){
    // duplicating the string as it is declared as a constant.
    char * newPath = strdup(filename);

    ppReturn ret;
    // If the path is invalid then it returns -1.
    if(parsePath(newPath, &ret) != 0){
        return -1;
    }

    // If the directory doesn't exist in the parent it will fail.
    // and return -1.
    if(ret.deNum == -1){
        return -1;
    }

    // The function will retuern the opposite of the isDir which
    // describes if the corresponding directory entry is a file or not.
    return !ret.parent[ret.deNum].isDirectory;
}