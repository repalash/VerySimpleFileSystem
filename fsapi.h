#include "basic.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#ifndef FSAPI_H
#define FSAPI_H

int writeFile(int disk, char* filename, void* block);
int readFile(int disk, char* filename, void* block);

#endif