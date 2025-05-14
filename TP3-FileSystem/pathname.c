
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    if (pathname == NULL || pathname[0] == '\0') {
        return -1;
    }

    int dirinumber = 1; // inicio en el directorio raíz

    char path_copy[strlen(pathname) + 1]; // me armo una copia del pathname
    strcpy(path_copy, pathname);
    char *token = strtok(path_copy, "/");

    while (token != NULL) { // recorro toda la ruta
        struct direntv6 dirEnt;

        int res = directory_findname(fs, token, dirinumber, &dirEnt); // busco el nombre en el directorio
        if (res < 0) {
            return -1;
        }

        dirinumber = dirEnt.d_inumber; // paso al siguiente directorio

        token = strtok(NULL, "/");
    }
    
    return dirinumber;
}
