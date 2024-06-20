/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha
* Student IDs:: 922180763
* GitHub-Name:: Dextron04
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: fs_readdir.c
*
* Description:: This file implements a function to read directory entries 
* and provide information about each entry.
*
**************************************************************/

#include "mfs.h"
#include "fileSystem.h"

struct fs_diriteminfo *fs_readdir(fdDir *dirp){

    // malloc size for the fs_diriteminfo that will contain information about
    // the directory that has been opened.
    struct fs_diriteminfo * fs_info;
    fs_info = malloc(dirp->entryCount * sizeof(struct fs_diriteminfo));

    // this will loop until it reaches tha max number of entry count and will stop
    // as soon as it finds an empty directory entry or reaches the entryCount.
    for(int i = dirp->dirEntryPosition; i < dirp->entryCount; i++){
        if(strcmp(dirp->directory[i].dirName, "") != 0){
            strncpy(fs_info->d_name, dirp->directory[i].dirName, sizeof(fs_info->d_name));
            
            // set the fileType to either directory or a file
            // using the tags FT_DIRECTORY or FT_REGFILE.
            if(dirp->directory[i].isDirectory == 1){
                fs_info->fileType = FT_DIRECTORY;
            } else{
                fs_info->fileType = FT_REGFILE;
            }

            dirp->dirEntryPosition = i + 1;
            fs_info->d_reclen = sizeof(struct fs_diriteminfo);

            // Will return infor about the entry that has been read.
            return fs_info;
        }
    }

    // Return NULL if the loop fails and no more directory entries are present.
    return NULL;
    
}