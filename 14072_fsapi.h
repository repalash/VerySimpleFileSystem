#include "14072_basic.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

#ifndef FSAPI_H
#define FSAPI_H

#define ROOT_INUMBER 2

typedef struct inode_t {
	size_t size; //no of bytes in this file
	time_t time; //last access time
	time_t ctime; //creation time
	int blocks; //no of blocks allocated to file
	int block[15]; //pointers to blocks (block number basically)
} inode_t;

int writeFile(int disk, char* filename, void* block);
int readFile(int disk, char* filename, void* block);

#endif