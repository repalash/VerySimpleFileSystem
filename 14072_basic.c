#include "14072_basic.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define BLOCK_SIZE 4096 //bytes
#define INODE_SIZE 256  //bytes

typedef struct superblock{
	int n_inodes;
	int n_datablocks;
	int inode_bitmap_start;
	int data_bitmap_start;
	int inode_start;
	int data_start;
	int magic_number;
} superblock;

int createSFS(char* filename, int nbytes){
	
}

int readData(int disk, int blockNum, void* block){

}

int writeData(int disk, int blockNum, void* block){

}