#ifndef BASIC_H
#define BASIC_H

int createSFS(char* filename, int nbytes);
int readData(int disk, int blockNum, void* block);
int writeData(int disk, int blockNum, void* block);

#endif