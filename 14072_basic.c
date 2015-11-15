#include "14072_basic.h"

int createSFS(char* filename, int nbytes){
	int MAX_BLOCKS = BLOCK_SIZE*8 + INODE_BLOCKS+DATA_BITMAP_BLOCKS+INODE_BITMAP_BLOCKS+1;
	if(nbytes > MAX_BLOCKS*BLOCK_SIZE || nbytes < MIN_BLOCKS*BLOCK_SIZE)
			return -1;

	//Creating file
	int file_system_id = open(filename, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH); //Create file with permission rw-rw-r-- 
	if(file_system_id==-1) return -1;

	// Writing zeros
	char zero = 0;
	int bytes = nbytes;
	while(bytes--)
		if(write(file_system_id, &zero, 1)<=0)
			return -1;

	//Initiating superblock
	struct superblock superblock;
	superblock.n_inodes = INODE_BLOCKS * BLOCK_SIZE / INODE_SIZE;
	superblock.n_datablocks = nbytes/BLOCK_SIZE - INODE_BLOCKS - DATA_BITMAP_BLOCKS - INODE_BITMAP_BLOCKS - 1;
	superblock.inode_bitmap_start = BLOCK_SIZE * 1;
	superblock.data_bitmap_start = superblock.inode_bitmap_start + BLOCK_SIZE * INODE_BITMAP_BLOCKS; 
	superblock.inode_start = superblock.data_bitmap_start + BLOCK_SIZE * DATA_BITMAP_BLOCKS;
	superblock.data_start = superblock.inode_start + BLOCK_SIZE * INODE_BLOCKS;
	superblock.magic_number = 69;

	//Write superblock
	if(lseek(file_system_id, 0, SEEK_SET)==-1)
		return -1;
	write(file_system_id, &superblock, sizeof(superblock));	

	return file_system_id;
}

int readData(int disk, int blockNum, void* block){

}

int writeData(int disk, int blockNum, void* block){

}