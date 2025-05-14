
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
    
    if (pathname == NULL || pathname[0] != '/') {
        return -1;
    }

    int inode_number = 1;  // Start at root inode
    char pathcopy[strlen(pathname) + 1];
    strcpy(pathcopy, pathname);

    char *token = strtok(pathcopy, "/");
    while (token != NULL) {
        struct direntv6 entry;
        int result = directory_findname(fs, token, inode_number, &entry);
        if (result != 0) {
            return -1; // Not found or error
        }
        inode_number = entry.d_inumber;
        token = strtok(NULL, "/");
    }

    return inode_number;
}
