#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int directory_findname(struct unixfilesystem *fs, const char *name,
  int dirinumber, struct direntv6 *dirEnt) {

  struct inode inodo;
  int err = inode_iget(fs, dirinumber, &inodo);
  if (err < 0) {
    return -1;
  }

  if ((inodo.i_mode & IALLOC) == 0 || (inodo.i_mode & IFMT) != IFDIR) { // me fijo si el inode es un directorio
    return -1;
  }

  int filesize = inode_getsize(&inodo);
  if (filesize < 0 || filesize == 0) {
    return -1;
  }

  int block_count = (filesize + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;

  for (int i = 0; i < block_count; i++) { // recorro todos los bloques del directorio
    unsigned char buffer[DISKIMG_SECTOR_SIZE];
    int bytes = file_getblock(fs, dirinumber, i, buffer);
    if (bytes < 0) return -1;

    int entries = bytes / sizeof(struct direntv6); // calculo el numero de entradas en el bloque
    struct direntv6 *entry = (struct direntv6 *)buffer;

    for (int j = 0; j < entries; j++) { // recorro todas las entradas del bloque
      // comparo el nombre de la entrada con el nombre que busco
      if (strncmp(entry[j].d_name, name, 256) == 0) { // CHEQUEAR LOS 256
          *dirEnt = entry[j];
          return 0;
      }
    }
  }
  return -1;
}
