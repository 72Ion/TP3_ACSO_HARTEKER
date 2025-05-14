#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"
#include "unixfilesystem.h"

#define INODES_PER_SEC (DISKIMG_SECTOR_SIZE / sizeof(struct inode))
#define ADDRESSES_PER_BLK (DISKIMG_SECTOR_SIZE / sizeof(uint16_t))

/**
 * TODO
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {

    if (inumber < 1) return -1;
    int inode_idx = inumber - 1;
    int sector = INODE_START_SECTOR + inode_idx / INODES_PER_SEC;
    char buffer[DISKIMG_SECTOR_SIZE];

    if (diskimg_readsector(fs->dfd, sector, buffer) < 0 ) {
        return -1;
    }

    struct inode *inode_tab = (struct inode *)buffer;
    *inp = inode_tab[inode_idx % INODES_PER_SEC];
    return 0;
}

/**
 * TODO
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp,
    int blockNum) {  

        ////
        if (blockNum < 0) return -1;

        if (!(inp->i_mode & ILARG)) {
            if (blockNum >= 8) return -1;
            return inp->i_addr[blockNum];
        }
    
        // Archivo grande bloques indirectos
        if (blockNum < (int)(7 * ADDRESSES_PER_BLK))
        
        {
            int indirect_idx = blockNum /  ADDRESSES_PER_BLK;
            int offset = blockNum % ADDRESSES_PER_BLK;
            uint16_t indirect_block[ADDRESSES_PER_BLK];
    
            if (diskimg_readsector(fs->dfd, inp->i_addr[indirect_idx], indirect_block) < 0)
                return -1;

            return indirect_block[offset];
        }
    
        // Dbles indirectos
        int double_blockNum = blockNum - 7*ADDRESSES_PER_BLK;
        int double_indirect_idx = double_blockNum / ADDRESSES_PER_BLK;
        int offset = double_blockNum % ADDRESSES_PER_BLK;
    
        uint16_t double_indirect_block[ADDRESSES_PER_BLK];
        if (diskimg_readsector(fs->dfd, inp->i_addr[7], double_indirect_block) < 0)
            return -1;
    
        uint16_t indirect_block[ADDRESSES_PER_BLK];
        if (diskimg_readsector(fs->dfd, double_indirect_block[double_indirect_idx], indirect_block) < 0)
            return -1;
    
        return indirect_block[offset];
    return 0;
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
