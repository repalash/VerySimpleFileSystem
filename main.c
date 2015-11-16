#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "basic.h"
#include "fsapi.h"
#include "diagnostic.h"

int main(){
	//Testing BASIC APIs
	int disk = createSFS("test_disk", 64*BLOCK_SIZE);
	printf("File descripter/Error code = %d\n", disk);
	int k;
	printf("Writing arbitarily to block 20\n");
	char *write_block = malloc(10);
	strcpy(write_block, "Hi this is block 20");
	writeData(disk, 20, write_block);
	printf("Reading block 20\n");
	char* read_block = malloc(10);
	readData(disk, 20, read_block);
	printf("%s\n", read_block);

	//Testing FS APIs
	printf("Creating 2 files /foo/a.txt and /bar\n");
	char* file_data = malloc(50);
	strcpy(file_data, "This is the foo file..");
	writeFile(disk, "/foo/a.txt", file_data);
	strcpy(file_data, "Say hello to bar file!! Some more chars here.");
	writeFile(disk, "/bar", file_data);

	char* data = malloc(1000);

	printf("Reading bar:\n");
	k = readFile(disk, "/bar", data);
	if(k==-5)
		printf("Return Value = -5 => Does not exist\n");
	else printf("%s\n", data);

	printf("Reading foo/a.txt:\n");
	k = readFile(disk, "/foo/a.txt", data);
	if(k==-5)
		printf("Return Value = -5 => Does not exist\n");
	else printf("%s\n", data);

	printf("Reading something else say foobar:\n");
	k = readFile(disk, "/foobar", data);
	if(k==-5)
		printf("Return Value = -5 => Does not exist\n");
	else printf("%s\n", data);

	printf("Overwriting the /foo/a.txt file:\n");
	strcpy(file_data, "This is the overwritten data in the foo file..");
	writeFile(disk, "/foo/a.txt", file_data);
	
	printf("Reading foo/a.txt:\n");
	k = readFile(disk, "/foo/a.txt", data);
	if(k==-5)
		printf("Return Value = -5 => Does not exist\n");
	else printf("%s\n", data);

	//Testing diagnostic APIs
	print_FileList(disk);
	print_inodeBitmaps(disk);
	print_dataBitmaps(disk);

	return 0;
}