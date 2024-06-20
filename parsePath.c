/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha, Hayden Coke, Eric Khuong, 
*        Thanh Duong
* Student IDs:: 922180763, 921741974, 923406338, 922438176
* GitHub-Name:: Dextron04, crowcode17, ekhuong, DanielDoubleDx
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: parsePath.c
*
* Description:: This file implements functions for parsing file paths 
* and locating directory entries, aiding in directory navigation and file access.
*
**************************************************************/

#include "fileSystem.h"

// Will find the relavent directory entry from the name passed in.
int findInDirectory(char * name, DirectoryEntry * parent){
    if(name == NULL || parent == NULL){
        return -1;
    }
    
    int entryCount = getEntryCount(parent);
    for(int i = 0; i < entryCount; i++) {
        if(strcmp(parent[i].dirName, name) == 0) {
            return i;
        }
    }

    return -1; 
}

// check if DE is a directory
// returns 1 if DE is a directory, 0 if not, -1 if the DE is NULL
int isDirectory(DirectoryEntry * DE){
    if(DE == NULL){
        return -1;
    }
    return DE->isDirectory;
}

// returns a 0 if the pathname is valid
int parsePath(char * path, ppReturn * ret){
    if(strlen(path) > MAX_PATH_SIZE) {
        printf("[ERROR] Path given exceeds max path size. Shorten your path.\n");

        return -1;
    }

    char * pathname = strdup(path);
    if(pathname == NULL){
        printf("[ERROR] No path given.\n");

        return -1;
    }

    DirectoryEntry * startParent;
    DirectoryEntry * currentParent;
    DirectoryEntry * tempParent;
    int deNum;

    // check - did the user pass in a relative path or an absolute path?
    if(pathname[0] == '/'){
        // the path the user gave was an absolute path, so parsing starts from the root
        startParent = RootDir;
        if(startParent == NULL){
            // printf("(PP) startParent = RootDir; returned NULL");
        }
    }
    else {
        // the user provided a relative path, so parsing starts from the current working directory
        startParent = CurrentWorkingDir;
    }

    currentParent = startParent;

    char * token;
    char * token2;
    
    // only way to break out of this loop is by returning from this function
    token = strtok(pathname, "/");
    while(true){

        // printf("(PP) token: %s\n", token);

        if(token == NULL){
            // If only a '/' was passed in.
            if(pathname[0] == '/'){
                ret->parent = startParent;
                ret->deNum = 0;
                return -2;
            }
            else {
                return -1;
            }
        }

        // find the index of the next directory in the parent directory
        deNum = findInDirectory(token, currentParent);
        token2 = strtok(NULL, "/");

        if (token2 != NULL) {
            if (deNum == -1) {
                // invalid path - directory doesn't exist in parent
                return -1;
            }
            
            if (!isDirectory(&currentParent[deNum])) {
                // invalid path - token is not a directory
                return -1;
            }
            
            // at this point we know token2 is a valid directory
            // bring directory into memory
            tempParent = loadDirectory(&currentParent[deNum]);

            // compare pointers
            if (currentParent != startParent) {
                // free old directory
                free(currentParent);
            }

            currentParent = tempParent;
            token = token2;

        } else {
            // token was last element
            ret->parent = currentParent;
            ret->deNum = deNum;
            ret->lastElement = token;

            return 0;
        }
    }
}
