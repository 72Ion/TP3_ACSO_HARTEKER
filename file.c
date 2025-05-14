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
    struct inode file_inode;

    if (inode_iget(fs, inumber, &file_inode) < 0)
        return -1;
    if ((file_inode.i_mode & IALLOC) == 0)
        return -1;

    int sector = inode_indexlookup(fs, &file_inode, blockNum);

    if (sector == -1)
        return -1;
    if (diskimg_readsector(fs->dfd,sector,buf) < 0)
        return -1;

    // Cuantos bytes validos hay en el bloque.
    int file_size = inode_getsize(&file_inode);
    int offset = blockNum*DISKIMG_SECTOR_SIZE;

    if (file_size > offset+DISKIMG_SECTOR_SIZE)
        return DISKIMG_SECTOR_SIZE;

    else if (file_size > offset)
        return file_size -offset;

    else
        return 0;
}

