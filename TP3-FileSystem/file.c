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
	struct inode inode1;
	int err = inode_iget(fs, inumber, &inode1);
	if (err < 0) {
        return -1;
    }

	int sector = inode_indexlookup(fs, &inode1, blockNum);
	if (sector < 0) {
        return -1;
    }

	int read_err = diskimg_readsector(fs->dfd, sector, buf);
	if (read_err < 0) {
        return -1;
    }

    int total_bytes = inode_getsize(&inode1);
	if (total_bytes < 0) {
        return -1;
    }
    
	int total_blocks = total_bytes / DISKIMG_SECTOR_SIZE;
	if (blockNum == total_blocks) {
		return total_bytes % DISKIMG_SECTOR_SIZE;
	} else {
		return DISKIMG_SECTOR_SIZE;
	}
}

