/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Eric Khuong
* Student IDs:: 923406338
* GitHub-Name:: ekhuong
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: fs_setcwd.c
*
* Description:: This file implements a function to set the current working 
* directory in a file system, allowing users to navigate directories efficiently 
* by updating the current directory path.
*
**************************************************************/

#include "mfs.h"
#include "fileSystem.h"

int buildString(char * newPath, char * pathname);

// sets the current working directory based on a given path from the user
// the path may or may not be invalid, so setcwd both checks the path
// and cleans up/simplifies the actual path string
int fs_setcwd(char *pathname){
    // no path given
    if(pathname == NULL){
        return -1;
    }

    // did the use just pass in the root?
    if(strcmp(pathname, "/") == 0){
        if(CurrentWorkingDir != RootDir){
            free(CurrentWorkingDir);

            CurrentWorkingDir = RootDir;
            strcpy(CWDPathname, "/\0");
        }

        // all done- no need to validate or clean up string
        return 0;
    }

    ppReturn parseHold;
    char * newPath = strdup(pathname);

    // check if this is a valid path
    if(parsePath(newPath, &parseHold) != 0){
        return -1;
    }

    // did the path the user pass in end in a slash?
    // 
    if(strcmp(parseHold.lastElement, "/") == 0){
        if(CurrentWorkingDir != RootDir){
            free(CurrentWorkingDir);
        }

        CurrentWorkingDir = RootDir;
        strcpy(CWDPathname, "/\0");

        return 0;
    }

    // if the denum from parsepath is -1, then the path was valid but
    // the element does not exist. The user cannot change directory
    // into a directory that does not exist.
    if(parseHold.deNum == -1){
        return -1;
    }

    // cannot change directory into a file
    if(!parseHold.parent[parseHold.deNum].isDirectory){
        return -1;
    }

    DirectoryEntry * holdDE;

    if(parseHold.parent[parseHold.deNum].location == RootDir->location){
        holdDE = RootDir;
    } else{
        holdDE = loadDirectory(&parseHold.parent[parseHold.deNum]);
    }

    if(CurrentWorkingDir != RootDir){\
        free(CurrentWorkingDir);
    }
    
    CurrentWorkingDir = holdDE;

    // at this point, we've done everything the file system itself needs
    // to manage the new current working directory.
    // now we convert the string that the user gave us into a usable path
    // and use that to set the cwd pathname.

    return buildString(newPath, pathname);
}

// the path may be a valid path, but that doesn't mean it's necessarily usable
// for example, the user may have done "cd ../.." which is valid but not informative
// as a current working directory. 
// the goal is to rebuild the path string to be usable when the user prints the current
// working directory
int buildString(char * newPath, char * pathname) {
    int pathInt = 0;
    int index = 0;
    char * newString = malloc(5000);
    char * token;
    char ** pathVector = malloc(5000); // vector of tokens
    char ** newPathVector = malloc(5000); // vector of actual path tokens

    if(newString == NULL) {
        printf("[ERROR] MALLOC FAILED\n");
        return -3;
    }

    if(pathVector == NULL) {
        printf("[ERROR] MALLOC FAILED\n");
        return -3;
    }

    // check if user passed in a RELATIVE path
    if(pathname[0] != '/'){
        char* cPathCpy = strdup(CWDPathname);
        token = strtok(cPathCpy, "/");

        while(token != NULL){\
            pathVector[pathInt] = token;
            token = strtok(NULL, "/");
            pathInt++;
        }
    }

    // begin tokenizing
    token = strtok(newPath, "/");
    
    while(token != NULL){
        pathVector[pathInt] = token;
        token = strtok(NULL, "/");
        pathInt++;
    }

    if(newPathVector == NULL) {
        printf("[ERROR] MALLOC FAILED\n");
        return -3;
    }

    memset(newString, 0, 5000);
    
    newString[0] = '/';

    // keep track of the . and .. parts of the path
    for(int i = 0; i < pathInt; i++){
        if(strcmp(pathVector[i], "..") == 0){
            if(index != 0){
                index--;
            }
        } else if(strcmp(pathVector[i], ".") != 0){
            newPathVector[index] = pathVector[i];
            index++; 
        }
    }

    // concatenate the vector of tokens to create a path
    for(int i=0; i<index;i++){
        strncat(newString, newPathVector[i], strlen(newPathVector[i]));
        strncat(newString, "/", sizeof("/"));
    }

    // did the user pass an absolute path?
    if(pathname[0] == '/'){
        free(CWDPathname);
        CWDPathname = strdup(newString);

        free(newString);
        free(pathVector);
        free(newPathVector);

        return 0;
    }
    
    if(CWDPathname[strlen(CWDPathname)-1] != '/'){
        strncat(CWDPathname, "/", sizeof("/"));
        strncat(CWDPathname, newString, strlen(newString));

        free(newString);
        free(pathVector);
        free(newPathVector);

        return 0;
    }

    // finally set the current working directory pathname
    CWDPathname = strdup(newString);

    free(newString);
    free(pathVector);
    free(newPathVector);
    
    return 0;
}

