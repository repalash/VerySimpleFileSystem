#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "14072_basic.h"
#include "14072_fsapi.h"
#include "14072_diagnostic.h"

int main(){
	int disk = createSFS("test_disk", 80*BLOCK_SIZE);
	struct superblock superblock;
	if(lseek(disk, 0, SEEK_SET)==-1)
		return -1;
	read(disk, &superblock, sizeof(superblock));
	printf("%d\n", superblock.magic_number);

	char *write_block = malloc(10);
	strcpy(write_block, "Palash");
	writeData(disk, 20, write_block);
	char* read_block = malloc(10);
	readData(disk, 20, read_block);
	printf("%s\n", read_block);

	return 0;
}