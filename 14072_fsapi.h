#ifndef FSAPI_H
#define FSAPI_H

int writeFile(int disk, char* filename, void* block);
int readFile(int disk, char* filename, void* block);

#endif