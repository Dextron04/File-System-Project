/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha
* Student IDs:: 922180763,
* GitHub-Name:: Dextron04
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: fs_stat.c
*
* Description:: BThis file implements a function to retrieve 
* file status information based on the provided file path.
*
**************************************************************/

#include "mfs.h"
#include "fileSystem.h"

int fs_stat(const char *path, struct fs_stat *buf){
    // if either the path or the buffer is NULL the function
    // will fail and return a -1.
    if(path == NULL || buf == NULL){
        return -1;
    }    

    ppReturn pRet;
    // duplicating the string as it is declared as a constant.
    char * newPath = strdup(path);
    int p = parsePath(newPath, &pRet);

    // If the path is invalid or the directory entry doesn't exist in the
    // parent the function will fail and return a -1.
    if(pRet.deNum == -1 || p != 0){
        return -1;
    }

    // The buffer will have the releavent data passed in to it 
    // about the directory entry and since it is a pass by reference 
    // the updated values in the buffer will be used by the rest of the
    // fucntions.
    buf->st_size = pRet.parent[pRet.deNum].size;
    buf->st_blocks = bytesToBlocks(pRet.parent[pRet.deNum].size);
    buf->st_blksize = BlockSize;
    buf->st_createtime = pRet.parent[pRet.deNum].dateCreated;
    buf->st_modtime = pRet.parent[pRet.deNum].dateModified;

    // On a successful execution the function will return a status of 0
    // indicating no errors encountred during execution.
    return 0;
}