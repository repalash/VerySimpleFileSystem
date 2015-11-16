#include "14072_fsapi.h"

int writeFile(int disk, char* filename, void* block){ 
	
	//Take sizeof(mem_block) into account while putting data into blocks

}

int readFile(int disk, char* filename, void* block){
	//Read the superblock
	struct superblock superblock;
	if(readData(disk, 0, &superblock)==-1)
		return -1;

	//Get root inode
	int root_inode_block = superblock.inode_start / BLOCK_SIZE + (ROOT_INUMBER * INODE_SIZE) / BLOCK_SIZE;
	void* root_inode_block_data = malloc(BLOCK_SIZE);
	if(readData(disk, root_inode_block, root_inode_block_data)==-1)
		return -1;
	inode_t* root_inode = (inode_t*)( (char*)root_inode_block_data + (ROOT_INUMBER * INODE_SIZE) % BLOCK_SIZE );

	//Search for file
	int inumber = ROOT_INUMBER;
	int filefound = 0; //is file found?
	void* root_data = malloc(BLOCK_SIZE); 
	for(int i=0; i<root_inode->blocks && !filefound; i++){
		int datablock = root_inode->block[i];
		if(readData(disk, datablock, root_data)==-1)
			return -1;
		char* ptr = (char*)root_data;
		char* file = malloc(64);  //Max filename length = 64
		int n;
		while(sscanf(ptr, "%63[^\n]%n", file, &n) == 1 && !filefound){
			inumber++;
			if(!strcmp(filename, file))
				filefound = 1;
			ptr+=n;
			if(*ptr!='\n') break;
			while(*ptr=='\n') ++ptr;
		}
	}
	if(!filefound) //File does not exist
		return -1;

	//Get file inode
	int inode_block = superblock.inode_start / BLOCK_SIZE + (inumber * INODE_SIZE) / BLOCK_SIZE;
	void* inode_block_data = malloc(BLOCK_SIZE);
	if(readData(disk, inode_block, inode_block_data)==-1)
		return -1;
	inode_t* inode = (inode_t*)( (char*)inode_block_data + (inumber * INODE_SIZE) % BLOCK_SIZE );

	//Read file to block
	char* data = malloc(BLOCK_SIZE); 
	char* ptr = (char*)block;
	for(int i=0; i<inode->blocks; i++){
		int datablock = inode->block[i];
		int bytes = readData(disk, datablock, data);
		inode->time = time (NULL); 
		if(bytes==-1) return -1;
		while(bytes--){
			*ptr=*data;
			ptr++;
			data++;
		}
	}

	return inode->size;

}