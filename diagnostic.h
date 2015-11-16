#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <malloc.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#include "basic.h"
#include "fsapi.h"

#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

int print_inodeBitmaps(int fileSystemId);
int print_dataBitmaps(int fileSystemId);
int print_FileList(int fileSystemId);
int print_inode(int disk, int inumber);

#endif