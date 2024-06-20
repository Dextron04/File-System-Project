/**************************************************************
* Class::  CSC-415-02 Spring 2024
* Name:: Tushin Kulshreshtha, Hayden Coke, Eric Khuong, 
*        Thanh Duong
* Student IDs:: 922180763, 921741974, 923406338, 922438176
* GitHub-Name:: Dextron04, crowcode17, ekhuong, DanielDoubleDx
* Group-Name:: Something Simple
* Project:: Basic File System
*
* File:: b_io.h
*
* Description:: Interface of basic I/O Operations
*
**************************************************************/

#ifndef _B_IO_H
#define _B_IO_H

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>			
#include <string.h>			
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "FileControlBlock.h"

#define B_CHUNK_SIZE 512

typedef int b_io_fd;

b_io_fd b_open (char * filename, int flags);
int b_read (b_io_fd fd, char * buffer, int count);
int b_write (b_io_fd fd, char * buffer, int count);
int b_seek (b_io_fd fd, off_t offset, int whence);
int b_close (b_io_fd fd);

int readPreliminaryBufferCopy(b_fcb * fcb, int byteCount, char ** buffer);
int readDirectFileCopy(b_fcb * fcb, int byteCount, char ** buffer, int callIndex);
int readSecondaryBufferCopy(b_fcb * fcb, int byteCount, char ** buffer, int callIndex);

int writePreliminaryBufferCopy(b_fcb * fcb, int byteCount, char * buffer, int offset);
int writeDirectFileCopy(b_fcb * fcb, int byteCount, char * buffer, int callIndex);
int writeSecondaryBufferCopy(b_fcb * fcb, int byteCount, char * buffer, int callIndex);

int getBlockLocation(b_fcb * fcb, int blocks);
int getFilePosition(b_fcb fcb);

int checkGrow(b_fcb * fcb, int count);
int growSpace(b_fcb * fcb, int bytesNeeded);
int shrinkSpace(b_fcb * fcb);

#endif

