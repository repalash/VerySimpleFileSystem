#include "diagnostic.h"

int print_inodeBitmaps(int fileSystemId){
	printf("Printing inode bitmap.\n");

	struct superblock superblock;
	if(readData(fileSystemId, 0, &superblock)==-1)
		return -2;

	char* data = malloc(BLOCK_SIZE);
	if(readData(fileSystemId, superblock.inode_bitmap_start/BLOCK_SIZE, data)==-1) 
		return -1;
	for(int i=0; i<superblock.n_inodes/8; i++){
		for(int j=0; j<8; j++){
			printf("%d %s\t", i*8 + j, (data[i]&(1<<j))?"1":"0");
		}
		printf("\n");
	}
}

int print_dataBitmaps(int fileSystemId){
	printf("Printing data bitmap.\n");

	struct superblock superblock;
	if(readData(fileSystemId, 0, &superblock)==-1)
		return -2;

	char* data = malloc(BLOCK_SIZE);
	if(readData(fileSystemId, superblock.data_bitmap_start/BLOCK_SIZE, data)==-1) 
		return -1;
	for(int i=0; i<superblock.n_datablocks/8; i++){
		for(int j=0; j<8; j++){
			printf("%d %s\t", i*8 + j, (data[i]&(1<<j))?"1":"0");
		}
		printf("\n");
	}
}

int print_FileList(int fileSystemId){
	printf("Printing file list:\n");

	//Read the superblock
	struct superblock superblock;
	if(readData(fileSystemId, 0, &superblock)==-1)
		return -2;

	//Get root inode
	int root_inode_block = superblock.inode_start / BLOCK_SIZE + (ROOT_INUMBER * INODE_SIZE) / BLOCK_SIZE;
	void* root_inode_block_data = malloc(BLOCK_SIZE);
	if(readData(fileSystemId, root_inode_block, root_inode_block_data)==-1)
		return -3;
	inode_t* root_inode = (inode_t*)( (char*)root_inode_block_data + (ROOT_INUMBER * INODE_SIZE) % BLOCK_SIZE );

	//Search for file
	int inumber = ROOT_INUMBER;
	void* root_data = malloc(BLOCK_SIZE); 
	for(int i=0; i<root_inode->blocks; i++){
		int datablock = root_inode->block[i];
		int bytes = readData(fileSystemId, datablock, root_data);
		if(bytes==-1) return -4;
		for(int j=0; j<bytes/sizeof(struct dir_listing); j++){
			struct dir_listing *file = (struct dir_listing*)root_data+j;
			printf("%d %s\n", file->inumber, file->filename);
		}
	}

	free(root_inode_block_data);
	free(root_data);
}

int print_inode(int disk, int inumber){
	//Read the superblock
	struct superblock superblock;
	if(readData(disk, 0, &superblock)==-1)
		return -2;

	int inode_block = superblock.inode_start / BLOCK_SIZE + (inumber * INODE_SIZE) / BLOCK_SIZE;
	void* inode_block_data = malloc(BLOCK_SIZE);
	if(readData(disk, inode_block, inode_block_data)==-1)
		return -1;
	inode_t* inode = (inode_t*)( (char*)inode_block_data + (inumber * INODE_SIZE) % BLOCK_SIZE );

	printf("%d, %d, %d, %d\n", inode->size, inode->blocks, inode->block[inode->blocks-1], inode->time);
	return 0;
}
