Very Simple File System
=======================

A Very Simple FileSystem developed as the project for OS course. 
It creates a virtual filesystem on a file, and has methods to read and write on a data block, and read and write from a file.
I used Inodes and Bitmaps to keep track of free data blocks and implement the file system.
The inode for a file has the file metadata and upto 15 pointers to data blocks. 