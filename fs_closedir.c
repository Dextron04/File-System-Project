/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha, Hayden Coke, Eric Khuong, 
*        Thanh Duong
* Student IDs:: 922180763, 921741974, 923406338, 922438176
* GitHub-Name:: Dextron04, crowcode17, ekhuong, DanielDoubleDx
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: fs_closedir.c
*
* Description:: This file implements a function to close a directory 
* and freeing allocated memory.
*
**************************************************************/

#include "mfs.h"
#include "fileSystem.h"

int fs_closedir(fdDir *dirp){

    // will fail if the directory that is being closed is either the root directory
    // or the current working directory.
    if(dirp->directory == RootDir || dirp->directory == CurrentWorkingDir){
        return -2;
    }

    // freed the loaded directory in fdDir to clear up space
    free(dirp->directory);
    
    // if a diriteminfo structure exists in the dirp it will free that
    if(dirp->di){
        free(dirp->di);
        dirp->di = NULL;
    }

    // Finally will free the fdDir structure.
    free(dirp);
    
    // set the pointer to NULL
    dirp = NULL;

    // Will return 0 if the close was successful.
    if(dirp == NULL){
        return 0;
    } else {
        return -1;
    }
}