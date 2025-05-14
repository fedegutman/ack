
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

    int directory = 1; // inicio en el directorio raíz
    char path_copy[strlen(pathname) + 1]; // me armo una copia de la ruta [+1 para el \0]
    strcpy(path_copy, pathname); // copio la ruta
    char *component = strtok(path_copy, "/");

    while (component != NULL) { // recorro toda la ruta
        struct direntv6 dirent;

        int res = directory_findname(fs, component, directory, &dirent); // busco el nombre en el directorio
        if (res < 0) {
            return -1;
        }

        directory = dirent.d_inumber;
        component = strtok(NULL, "/"); // avanzo
    }

    return directory;
}
