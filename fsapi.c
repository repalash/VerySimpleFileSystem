#include "fsapi.h"

int writeFile(int disk, char* filename, void* block){  //block must be in heap
	if(strlen(filename)>64) //Max filename length = 64
		return -1;
	size_t data_size = malloc_usable_size(block);
	if(data_size>15*BLOCK_SIZE)
		return -1;

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

	//TODO: Cater for INODE_BITMAP_BLOCKS > 1
	//Search for file
	int filefound = 0; //is file found?
	void* root_data = malloc(BLOCK_SIZE); 
	int n=0;
	for(int i=0; i<root_inode->blocks && !filefound; i++){
		int datablock = root_inode->block[i];
		int bytes = readData(disk, datablock, root_data);
		if(bytes==-1) return -4;
		int j=0;
		for(n; n<bytes/sizeof(struct dir_listing); n++, j++){
			struct dir_listing *file = (struct dir_listing*)root_data+j;
			// printf("%s %d\n", file->filename, file->inumber);
			if(!strcmp(filename, file->filename)){
				filefound = 1;
				n = file->inumber;
				break;
			}
		}
	}
	int inumber = 0;
	if(filefound){
		inumber = n;
		printf("File found named %s with inode number %d.\n", filename, inumber);
	}
	else{ //File does not exist	
		  //Search for free inode
		char* data = malloc(BLOCK_SIZE);
		if(readData(disk, superblock.inode_bitmap_start/BLOCK_SIZE, data)==-1) 
			return -1;
		for(int i=0; i<superblock.n_inodes/8&&!inumber; i++)
			for(int j=0; j<8; j++)
				if(!(data[i]&(1<<j))){ //bit is 0
					data[i] |= (1<<j);
					inumber = i*8 + j;
					break;
				}
		//Write back bitmap
		if(writeData(disk, superblock.inode_bitmap_start/BLOCK_SIZE, data)==-1) 
			return -1;
		printf("File not found. Creating new one named %s and with inode number %d.\n",  filename, inumber);
	}

	//Get free inode
	int inode_block = superblock.inode_start / BLOCK_SIZE + (inumber * INODE_SIZE) / BLOCK_SIZE;
	void* inode_block_data;
	if(inode_block!=root_inode_block){
		inode_block_data = malloc(BLOCK_SIZE);
		if(readData(disk, inode_block, inode_block_data)==-1)
			return -1;
	}else{
		inode_block_data = root_inode_block_data;
	}
	inode_t* inode = (inode_t*)( (char*)inode_block_data + (inumber * INODE_SIZE) % BLOCK_SIZE );

	//Write Data
	char* ptr = (char*)block;
	char* data = malloc(BLOCK_SIZE); 
	inode->blocks = 0;
	for(int i=0; i<=data_size/(MAX_DATA_SIZE+1); i++){
		
		//Search for free data block
		if(readData(disk, superblock.data_bitmap_start/BLOCK_SIZE, data)==-1) 
			return -1;
		int dbnumber=0;
		for(int i=0; i<superblock.n_datablocks/8&&!dbnumber; i++)
			for(int j=0; j<8; j++)
				if(!(data[i]&(1<<j))){ //bit is 0
					data[i] |= (1<<j);
					dbnumber = i*8 + j + 1+INODE_BITMAP_BLOCKS+DATA_BITMAP_BLOCKS+INODE_BLOCKS;
					break;
				}
		//Write back bitmap
		if(writeData(disk, superblock.data_bitmap_start/BLOCK_SIZE, data)==-1) 
			return -1;

		//Write 
		if(i==data_size/(MAX_DATA_SIZE+1)){
			int k = writeData(disk, dbnumber, ptr);
			if(k==-1)return -1;
		}else{
			int k = writeData(disk, dbnumber, ptr);
			if(k==-1)return -1;
			ptr+=k;
		}

		//Update inode
		inode->blocks++;
		inode->block[i]=dbnumber;
	}
	//Update inode
	inode->size=data_size;
	inode->ctime = time(NULL);
	inode->time = time(NULL);
	if(writeData(disk, inode_block, inode_block_data)==-1)
		return -1;

	//Add file to root data
	int root_block = root_inode->block[root_inode->blocks-1];
	int root_bytes = readData(disk, root_block, root_data);
	if(root_bytes==-1)
		return -1;
	root_bytes = (root_bytes/sizeof(struct dir_listing))*sizeof(struct dir_listing);
	char* new_root_data = malloc(root_bytes+sizeof(struct dir_listing));
	memcpy(new_root_data, root_data, root_bytes);
	struct dir_listing dir_listing;
	dir_listing.inumber = inumber;
	strcpy(dir_listing.filename, filename);
	memcpy(new_root_data+root_bytes, &(dir_listing), sizeof(dir_listing));
	root_inode->size+=sizeof(dir_listing);
	if(writeData(disk, root_block, new_root_data)==-1)
		return -1;

	//Update root inode
	root_inode->time = time(NULL);
	if(writeData(disk, root_inode_block, root_inode_block_data)==-1)
		return -1;
	free(data);
	free(root_inode_block_data);
	free(root_data);
	if(root_inode_block!=inode_block)
		free(inode_block_data);

	return data_size;
}

int readFile(int disk, char* filename, void* block){
	if(strlen(filename)>64) //Max filename length = 64
		return -1;

	//Read the superblock
	struct superblock superblock;
	if(readData(disk, 0, &superblock)==-1)
		return -2;

	//Get root inode
	int root_inode_block = superblock.inode_start / BLOCK_SIZE + (ROOT_INUMBER * INODE_SIZE) / BLOCK_SIZE;
	void* root_inode_block_data = malloc(BLOCK_SIZE);
	if(readData(disk, root_inode_block, root_inode_block_data)==-1)
		return -3;
	inode_t* root_inode = (inode_t*)( (char*)root_inode_block_data + (ROOT_INUMBER * INODE_SIZE) % BLOCK_SIZE );

	//Search for file
	int inumber = ROOT_INUMBER;
	int filefound = 0; //is file found?
	void* root_data = malloc(BLOCK_SIZE); 
	int n=0;
	for(int i=0; i<root_inode->blocks && !filefound; i++){
		int datablock = root_inode->block[i];
		int bytes = readData(disk, datablock, root_data);
		if(bytes==-1) return -4;
		int j=0;
		for(n; n<bytes/sizeof(struct dir_listing); n++, j++){
			struct dir_listing *file = (struct dir_listing*)root_data+j;
			// printf("%s %d\n", file->filename, file->inumber);
			if(!strcmp(filename, file->filename)){
				filefound = 1;
				n = file->inumber;
				break;
			}
		}
	}
	if(!filefound) //File does not exist
		return -5;
	inumber = n;
	//Get file inode
	int inode_block = superblock.inode_start / BLOCK_SIZE + (inumber * INODE_SIZE) / BLOCK_SIZE;
	void* inode_block_data = malloc(BLOCK_SIZE);
	if(readData(disk, inode_block, inode_block_data)==-1)
		return -6;
	inode_t* inode = (inode_t*)( (char*)inode_block_data + (inumber * INODE_SIZE) % BLOCK_SIZE );

	//Read file to block
	char* data = malloc(BLOCK_SIZE); 
	char* dataptr = data;
	char* ptr = (char*)block;
	for(int i=0; i<inode->blocks; i++){
		int datablock = inode->block[i];
		int bytes = readData(disk, datablock, dataptr);
		inode->time = time (NULL); 
		if(bytes==-1) return -1;
		memcpy(ptr, dataptr, bytes);
		ptr+=bytes;
	}

	//Write back inode
	if(writeData(disk, inode_block, inode_block_data)==-1)
		return -7;

	int size = inode->size;

	free(root_inode_block_data);
	free(root_data);
	free(inode_block_data);
	free(data);

	return size;
}
