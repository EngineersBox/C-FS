#pragma once


#ifndef C_FS_CFS_H
#define C_FS_CFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "../thread/lock.h"
#include "../disk/disk.h"

#define MAGIC_NUMBER	   0xf0f03410
#define INODES_PER_BLOCK   128
#define POINTERS_PER_INODE 5
#define POINTERS_PER_BLOCK 1024

typedef struct SuperBlock {
    uint32_t magicNumber; // File system magic number
    uint32_t blocks;	  // Number of blocks in file system
    uint32_t iNodeBlocks; // Number of blocks reserved for inodes
    uint32_t iNodes;	  // Number of inodes in file system
} SuperBlock;

typedef struct INode {
    uint32_t valid;		                 // Whether or not inode is valid
    uint32_t size;		                 // Size of file
    uint32_t direct[POINTERS_PER_INODE]; // Direct pointers
    uint32_t indirect;	                 // Indirect pointer
} INode;

typedef union Block {
    SuperBlock super;			           // Superblock
    INode iNodes[INODES_PER_BLOCK];	       // Inode block
    uint32_t pointers[POINTERS_PER_BLOCK]; // Pointer block
    char data[BLOCK_SIZE];	               // Data block
} Block;

typedef struct CFS {
    // Member vars here
    __cfs_lock_t lock;
} CFS;

static void cfs_debug(CFS* cfs, Disk* disk);
static void cfs_format(CFS* cfs, Disk* disk);

bool cfs_mount(CFS* cfs, Disk* disk);

size_t cfs_create();
int cfs_destroy(CFS* cfs);

bool cfs_remove(CFS* cfs, size_t inumber);
size_t cfs_stat(CFS* cfs, size_t inumber);

size_t cfs_read(CFS* cfs, size_t inumber, char* src, size_t length, size_t offset);
size_t cfs_write(CFS* cfs, size_t inumber, char* dst, size_t length, size_t offset);

#ifdef __cplusplus
};
#endif

#endif //C_FS_CFS_H
