#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <malloc.h>

#ifndef BASIC_H
#define BASIC_H

#define BLOCK_SIZE 64 //bytes
#define INODE_SIZE 256  //bytes
#define INODE_BLOCKS 5
#define DATA_BITMAP_BLOCKS 1
#define INODE_BITMAP_BLOCKS 1
#define MIN_BLOCKS 64

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

int createSFS(char* filename, int nbytes);
int readData(int disk, int blockNum, void* block);
int writeData(int disk, int blockNum, void* block);

#endif