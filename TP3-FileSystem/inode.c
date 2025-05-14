#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"

/**
 * TODO
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
  if (inumber <= 0 || inp == NULL) {
    return -1;
  }

  inumber = inumber - 1; // la indexacion arranca en 1
  int inodes_per_sector = DISKIMG_SECTOR_SIZE / sizeof(struct inode); // cantidad de inodes por sector

  int sector = inumber / inodes_per_sector; // me devuelve el sector donde esta el inode
  int inode_position = inumber % inodes_per_sector; // busco la posicion del inode en el sector

  int sector_to_read = INODE_START_SECTOR + sector; // busco el sector a leer

  int fd = fs->dfd; // obtengo el file descriptor del filesystem
  struct inode *inodes = malloc(DISKIMG_SECTOR_SIZE); // reservo memoria para los inodes

  if (inodes == NULL) {
    return -1;
  }

  int err = diskimg_readsector(fd, sector_to_read, inodes); // leo el sector

  if (err < 0){
    free(inodes);
    return -1;
  }
  
  *inp = inodes[inode_position]; // me guardo el inode que buscaba
  free(inodes);
  return 0;
}

/**
 * TODO
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {  
  if (blockNum < 0 || inp == NULL) {
    return -1;
  }
  int fd = fs->dfd;
  int addressing = ((inp->i_mode & ILARG) == 0);
  if (addressing) {
    return inp->i_addr[blockNum];
  }

  int n_addresses = DISKIMG_SECTOR_SIZE / sizeof(uint16_t);
  int singly_indirect_blocks = n_addresses * 7;

  if (blockNum < singly_indirect_blocks) {
    int ind_block = blockNum / n_addresses;
    int ind_block_index = blockNum % n_addresses;
    uint16_t *addrs = malloc(DISKIMG_SECTOR_SIZE);
    if (addrs == NULL) {
      return -1;
    }
    int err = diskimg_readsector(fd, inp->i_addr[ind_block], addrs);
    if (err < 0) {
      free(addrs);
      return -1;	
    }
    int result = addrs[ind_block_index];
    free(addrs);
    return result;
  }

  int nblock = blockNum - singly_indirect_blocks;
  int double_ind_block_index1 = nblock / n_addresses;
  uint16_t *addresses1 = malloc(DISKIMG_SECTOR_SIZE);
  if (addresses1 == NULL) {
    return -1;
  }
  int block1_err = diskimg_readsector(fd, inp->i_addr[7], addresses1);
  if (block1_err < 0) {
    free(addresses1);
    return -1;
  }

  int sector_2 = addresses1[double_ind_block_index1];
  free(addresses1);

  int double_ind_block_index2 = nblock % n_addresses;
  uint16_t *addresses2 = malloc(DISKIMG_SECTOR_SIZE);
  if (addresses2 == NULL) {
    return -1;
  }
  int block2_err = diskimg_readsector(fd, sector_2, addresses2);
  if (block2_err < 0) {
    free(addresses2);
    return -1;
  }

  int result = addresses2[double_ind_block_index2];
  free(addresses2);
  return result;
}	

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}