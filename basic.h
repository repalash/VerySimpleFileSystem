#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <malloc.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#ifndef BASIC_H
#define BASIC_H

#define BLOCK_SIZE 1024 //bytes
#define INODE_SIZE 256  //bytes
#define INODE_BLOCKS 5
#define DATA_BITMAP_BLOCKS 1
#define INODE_BITMAP_BLOCKS 1
#define MIN_BLOCKS 64
#define ROOT_INUMBER 2
#define MAX_FILENAME_SIZE 64

struct superblock{
	int n_inodes;
	int n_datablocks;
	int inode_bitmap_start;
	int data_bitmap_start;
	int inode_start;
	int data_start;
	int magic_number;
};

struct mem_block { //written at the starting of each block
	size_t n_bytes;
};

typedef struct inode_t {
	size_t size; //no of bytes in this file
	time_t time; //last access time
	time_t ctime; //creation time
	int blocks; //no of blocks allocated to file
	int block[15]; //pointers to blocks (block number basically)
} inode_t;

struct dir_listing{
	int inumber;
	char filename[MAX_FILENAME_SIZE];
};

#define MAX_DATA_SIZE (BLOCK_SIZE-sizeof(struct mem_block))

int createSFS(char* filename, int nbytes);
int readData(int disk, int blockNum, void* block);
int writeData(int disk, int blockNum, void* block);

#endif