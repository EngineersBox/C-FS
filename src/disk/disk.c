#include "disk.h"

#include <stdlib.h>
#include <unistd.h>

Disk* disk_create() {
    Disk* disk = malloc(sizeof(*disk));
    if (disk == NULL) {
        return disk;
    }
    disk->descriptor = NULL;
    disk->blocks = 0;
    disk->reads = 0;
    disk->writes = 0;
    disk->mounts = 0;
    if (__cfs_lock_init(&disk->lock, PTHREAD_MUTEX_RECURSIVE) != 0) {
        return NULL;
    }
    return disk;
}

int dist_destroy(Disk* disk) {
    if (disk == NULL || disk->descriptor == NULL) {
        return 0;
    } else if (__cfs_lock_lock(&disk->lock) != 0) {
        return -1;
    }
    fclose(disk->descriptor);
    if (__cfs_lock_unlock(&disk->lock) != 0) {
        return -1;
    }
    free(disk);
    return 0;
}

int disk_open(Disk* disk, const char* path, size_t nblocks) {
    if (disk == NULL  || path == NULL) {
        return -1;
    } else if (__cfs_lock_lock(&disk->lock) != 0) {
        return -1;
    }
    if ((disk->descriptor = fopen(path, "w+")) == NULL) {
        __cfs_lock_unlock(&disk->lock);
        return -1;
    }
    if (ftruncate(fileno(disk->descriptor), nblocks * BLOCK_SIZE) != 0) {
        __cfs_lock_unlock(&disk->lock);
        return -1;
    }
    disk->blocks = nblocks;
    disk->reads = 0;
    disk->writes = 0;
    return __cfs_lock_unlock(&disk->lock);
}

size_t disk_size(Disk* disk) {
    if (disk == NULL || disk->descriptor == NULL) {
        return 0;
    }
    return disk->blocks;
}

bool disk_mounted(Disk* disk) {
    return disk->mounts > 0;
}

int disk_mount(Disk* disk) {
    if (disk == NULL) {
        return -1;
    } else if (__cfs_lock_lock(&disk->lock) != 0) {
        return -1;
    }
    disk->mounts++;
    return __cfs_lock_unlock(&disk->lock);
}

int disk_unmount(Disk* disk) {
    if (disk == NULL) {
        return -1;
    }  else if (__cfs_lock_lock(&disk->lock) != 0) {
        return -1;
    } else if (disk->mounts > 0) {
        disk->mounts--;
        return 0;
    }
    return __cfs_lock_unlock(&disk->lock) != 0 ? -1 : 1;
}

bool disk_verify_integrity(Disk* disk, int block, const char* data) {
    if (disk == NULL || disk->descriptor == NULL) {
        return false;
    } else if (block < 0) {
        return false;
    } else if (block >= (int) disk->blocks) {
        return false;
    } else if (data == NULL) {
        return false;
    }
    return true;
}

int disk_read(Disk* disk, int block, char* src) {
    if (disk != NULL && __cfs_lock_lock(&disk->lock) != 0) {
        return -1;
    } else if (!disk_verify_integrity(disk, block, src)) {
        __cfs_lock_unlock(&disk->lock);
        return -1;
    } else if (fseek(disk->descriptor, block * BLOCK_SIZE, SEEK_SET) < 0) {
        __cfs_lock_unlock(&disk->lock);
        return -1;
    } else if (fread(src, BLOCK_SIZE, 1, disk->descriptor) != 1) {
        __cfs_lock_unlock(&disk->lock);
        return -1;
    }
    disk->reads++;
    return __cfs_lock_unlock(&disk->lock);;
}

int disk_write(Disk* disk, int block, char* dst) {
    if (disk != NULL && __cfs_lock_lock(&disk->lock) != 0) {
        return -1;
    } else if (!disk_verify_integrity(disk, block, dst)) {
        __cfs_lock_unlock(&disk->lock);
        return -1;
    } else if (fseek(disk->descriptor, block * BLOCK_SIZE, SEEK_SET) < 0) {
        __cfs_lock_unlock(&disk->lock);
        return -1;
    } else if (fwrite(dst, BLOCK_SIZE, 1, disk->descriptor) != 1) {
        __cfs_lock_unlock(&disk->lock);
        return -1;
    }
    disk->writes++;
    return __cfs_lock_unlock(&disk->lock);;
}