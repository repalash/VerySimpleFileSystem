#include "basic.h"

int createSFS(char* filename, int nbytes){
	int MAX_BLOCKS = BLOCK_SIZE*8 + INODE_BLOCKS+DATA_BITMAP_BLOCKS+INODE_BITMAP_BLOCKS+1;
	if(nbytes > MAX_BLOCKS*BLOCK_SIZE || nbytes < MIN_BLOCKS*BLOCK_SIZE)
		return -1;

	//Creating file
	int file_system_id = open(filename, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH); //file with permission rw-rw-r-- 
	if(file_system_id==-1) return -2;

	// Writing zeros
	char zero = 0;
	int bytes = nbytes;
	while(bytes--)
		if(write(file_system_id, &zero, 1)<=0)
			return -3;

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
		return -4;
	write(file_system_id, &superblock, sizeof(superblock));	

	// TODO: Create root inode and data
	// Set bits 0,1,2 in inode bitmap and data bitmaps.
	char* data = malloc(BLOCK_SIZE);
	if(readData(file_system_id, superblock.inode_bitmap_start/BLOCK_SIZE, data)==-1) 
		return -5;
	data[0] |= (1<<0) | (1<<1) | (1<<2);
	if(writeData(file_system_id, superblock.inode_bitmap_start/BLOCK_SIZE, data)==-1) 
		return -6;
	if(readData(file_system_id, superblock.data_bitmap_start/BLOCK_SIZE, data)==-1) 
		return -7;
	data[0] |= (1<<0) | (1<<1) | (1<<2);
	if(writeData(file_system_id, superblock.data_bitmap_start/BLOCK_SIZE, data)==-1) 
		return -8;
	
	//Initial root dir inode entry
	int root_inode_block = superblock.inode_start / BLOCK_SIZE + (ROOT_INUMBER * INODE_SIZE) / BLOCK_SIZE;
	void* root_inode_block_data = malloc(BLOCK_SIZE);
	if(readData(file_system_id, root_inode_block, root_inode_block_data)==-1)
		return -9;
	inode_t* root_inode = (inode_t*)( (char*)root_inode_block_data + (ROOT_INUMBER * INODE_SIZE) % BLOCK_SIZE );
	root_inode->ctime = time(NULL);
	root_inode->time = time(NULL);
	root_inode->blocks = 1;
	root_inode->size = sizeof(struct dir_listing);
	root_inode->block[0] = ROOT_INUMBER+1+INODE_BITMAP_BLOCKS+DATA_BITMAP_BLOCKS+INODE_BLOCKS;
	if(writeData(file_system_id, root_inode_block, root_inode_block_data)==-1)
		return -10;
	
	//Inintialize root dir data
	int root_block = root_inode->block[root_inode->blocks-1];
	char* new_root_data = malloc(sizeof(struct dir_listing));
	struct dir_listing dir_listing;
	dir_listing.inumber = ROOT_INUMBER;
	strcpy(dir_listing.filename, ".");
	memcpy(new_root_data, &(dir_listing), sizeof(dir_listing));
	if(writeData(file_system_id, root_block, new_root_data)==-1)
		return -11;

	char* file_data = malloc(5);
	strcpy(file_data, "..");
	writeFile(file_system_id, "..", file_data);

	free(data);
	free(root_inode_block_data);
	free(new_root_data);
	free(file_data);
	
	return file_system_id;
}

int readData(int disk, int blockNum, void* block){ //block should be allocated
	if(lseek(disk, blockNum*BLOCK_SIZE, SEEK_SET)==-1)
		return -1;
	struct mem_block mem_block;
	if(blockNum==0) //if superblock
		mem_block.n_bytes = sizeof(struct superblock); 
	else if(blockNum<(1+INODE_BITMAP_BLOCKS+DATA_BITMAP_BLOCKS+INODE_BLOCKS)) //if inode block
		mem_block.n_bytes = BLOCK_SIZE;
	else if(read(disk, &mem_block, sizeof(mem_block))==-1) //Read no of bytes
		return -1;
	return read(disk, block, mem_block.n_bytes); //Read Data
}

int writeData(int disk, int blockNum, void* block){ //block should be in heap
	if(lseek(disk, blockNum*BLOCK_SIZE, SEEK_SET)==-1)
		return -1;
	struct mem_block mem_block;
	mem_block.n_bytes = malloc_usable_size(block);
	if(blockNum>=(1+INODE_BITMAP_BLOCKS+DATA_BITMAP_BLOCKS+INODE_BLOCKS)){ //if data block
		if(mem_block.n_bytes > MAX_DATA_SIZE)
			mem_block.n_bytes = MAX_DATA_SIZE;
		if(write(disk, &mem_block, sizeof(mem_block))==-1) //Write no of bytes
			return -1;
	}else
		mem_block.n_bytes = BLOCK_SIZE;
	return write(disk, block, mem_block.n_bytes); //Write data
}