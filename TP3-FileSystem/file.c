#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

/**
 * TODO
 */
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    if (fs == NULL || buf == NULL || inumber <= 0 || blockNum < 0) {
        return -1;
    }

    struct inode inode;
    if (inode_iget(fs, inumber, &inode) < 0) {
        return -1;
    }

    if ((inode.i_mode & IALLOC) == 0) {
        return -1;
    }

    int file_size = inode_getsize(&inode);
    if (file_size <= 0) {
        return 0;
    }

    int total_blocks = (file_size + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;
    if (blockNum >= total_blocks) {
        return -1;
    }

    int sector = inode_indexlookup(fs, &inode, blockNum);
    if (sector < 0) {
        return -1;
    }

    if (sector == 0) {
        memset(buf, 0, DISKIMG_SECTOR_SIZE);
    } else {
        int read_bytes = diskimg_readsector(fs->dfd, sector, buf);
        if (read_bytes != DISKIMG_SECTOR_SIZE) {
            return -1;
        }
    }

    int offset = blockNum * DISKIMG_SECTOR_SIZE;
    int remaining = file_size - offset;

    return (remaining > DISKIMG_SECTOR_SIZE) ? DISKIMG_SECTOR_SIZE : remaining;
}

