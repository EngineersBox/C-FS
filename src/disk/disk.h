#pragma once

#ifndef C_FS_DISK_H
#define C_FS_DISK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

#include "../thread/lock.h"

static const size_t BLOCK_SIZE = 4096;

typedef struct Disk {
    FILE* descriptor;
    size_t blocks;
    size_t reads;
    size_t writes;
    size_t mounts;
    __cfs_lock_t lock;
} Disk;

Disk* disk_create();
int disk_destroy(Disk* disk);

int disk_open(Disk* disk, const char* path, size_t nblocks);

size_t disk_size(Disk* disk);
bool disk_verify_integrity(Disk* disk, int blocknum, const char* data);

bool disk_mounted(Disk* disk);
int disk_mount(Disk* disk);
int disk_unmount(Disk* disk);

int disk_read(Disk* disk, int block, char* src);
int disk_write(Disk* disk, int block, char* dst);

#ifdef __cplusplus
};
#endif

#endif //C_FS_DISK_H
