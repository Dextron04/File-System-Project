/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha, Hayden Coke, Eric Khuong, 
*        Thanh Duong
* Student IDs:: 922180763, 921741974, 923406338, 922438176
* GitHub-Name:: Dextron04, crowcode17, ekhuong, DanielDoubleDx
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: fsInit.c
*
* Description:: Main driver for file system assignment.
*
* This file is where you will start and initialize your system
*
**************************************************************/

#include "fileSystem.h"
#include "b_io.h"

#define VCB_SIGNATURE 0x5353696D706C65 // SSimple
#define ROOT_ENTRY_COUNT 30

// externs used by the rest of the files in the file system.
VCB * Vcb;
int BlockSize;
DirectoryEntry * RootDir;
DirectoryEntry * CurrentWorkingDir;
char * CWDPathname;



// Initialize the VCB
int initializeVCB(int numberOfBlocks) {
	// set signature and blocks
	Vcb->signature = VCB_SIGNATURE;
	Vcb->blockCount = numberOfBlocks;

	// initialize and set FAT
	initializeFAT(numberOfBlocks); 
	// FAT freeChainHead is set within initializeFAT
	printf("(INIT) The freeChainHead is at: %d\n", Vcb->freeChainHead);
	Vcb->freeSpaceLocation = 1; // FAT comes immediately after VCB
	
	// intializing the FAT returns the first free location (FCH)
	// root comes immediately after FAT
	Vcb->rootLocation = Vcb->freeChainHead;

	// initialize root directory
	// the root directory is written to disk by the createDirectory function
	RootDir = createDirectory(ROOT_ENTRY_COUNT, NULL);

	Vcb->rootSize = bytesToBlocks(RootDir[0].size);

	// save FAT and VCB to disk (root is already saved)
	saveFAT();
}

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize) {
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);

	BlockSize = blockSize;
	Vcb = malloc(BlockSize);
	RootDir = malloc(bytesToBlocks(sizeof(DirectoryEntry) * 31) * BlockSize);

	// check disk's volume control block
	LBAread(Vcb, 1, 0);

	// Check the signature- if it matches, the volume has already been formatted
	// if it does not match, this is a new volume that must be initialized
	if(Vcb->signature == VCB_SIGNATURE){
		printf("Signature matched\n");

		// load FAT and Root Directory
		loadFAT(numberOfBlocks);
		loadRootDir();

		// error check- was the root directory successfully loaded?
		if(RootDir == NULL){
			printf("[ERROR] Could not load root directory\n");
			return -1;
		}
	} else{
		printf("Signature did not match, initializing VCB\n");

		initializeVCB(numberOfBlocks);
	}

	// on startup, the current working directory is the same as the root
	CurrentWorkingDir = RootDir;
	
	CWDPathname = malloc(MAX_PATH_SIZE);
	CWDPathname[0] = '/';
	CWDPathname[1] = '\0';

	return 0;
}


void exitFileSystem() {
	printf ("System exiting...\n");

	// make sure everything gets written to disk before exiting

	saveFAT();
	freeFAT();
	free(Vcb);
	free(CWDPathname);

	if(RootDir != CurrentWorkingDir){
		free(CurrentWorkingDir);
	}

	free(RootDir);

	printf("System exit complete.\nBye!\n");
}

