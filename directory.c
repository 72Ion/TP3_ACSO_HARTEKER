#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "direntv6.h"

#define MAX_NAMELEN_FILE_V6 14

/**
 * TODO
 */
int directory_findname(struct unixfilesystem *fs, const char *name,
		int dirinumber, struct direntv6 *dirEnt) {

      struct inode direc_inode;
      if (inode_iget(fs, dirinumber, &direc_inode) < 0 )
        return -1;
  
      // Validar que I node es directorio valido 
      if ((direc_inode.i_mode & IALLOC) == 0|| (direc_inode.i_mode & IFMT) != IFDIR)
        return -1;
  
      int file_size = inode_getsize(&direc_inode);
      int nblocks = (file_size + DISKIMG_SECTOR_SIZE - 1)/DISKIMG_SECTOR_SIZE;
  
      char block[DISKIMG_SECTOR_SIZE];
      for (int i = 0; i < nblocks ; i++) {
          int size = file_getblock(fs, dirinumber, i, block);
          if (size < 0) return -1;
  
          int entrys = size/sizeof(struct direntv6);

          struct direntv6 *entrysv6 = (struct direntv6 *)block;
          for (int j = 0; j < entrys; j++) {
              if (strncmp(entrysv6[j].d_name, name,MAX_NAMELEN_FILE_V6) ==  0) {

                  *dirEnt = entrysv6[j];
                  return 0;
              }
          }
      }
      return 1; // no fue encontrado
}
