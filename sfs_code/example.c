#include <driver.h>
#include <sfs_dir.h>
#include <sfs_superblock.h>
#include <sfs_inode.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
  /* declare a buffer that is the same size as a filesystem block */
  char raw_superblock[128];

  /* Create a pointer to the buffer so that we can treat it as a
     superblock struct. The superblock struct is smaller than a block,
     so we have to do it this way so that the buffer that we read
     blocks into is big enough to hold a complete block. Otherwise the
     driver_read function will overwrite something that should not be
     overwritten. */
  sfs_superblock *super = (sfs_superblock *)raw_superblock;

  /* open the disk image and get it ready to read/write blocks */
  driver_attach_disk_image("initrd", 128);

  /* CHANGE THE FOLLOWING CODE SO THAT IT SEARCHES FOR THE SUPERBLOCK */

  int i = 0;
  while(i < 128){
    driver_read(super, i);
    if(super->fsmagic == VMLARIX_SFS_MAGIC &&
     ! strcmp(super->fstypestr,VMLARIX_SFS_TYPESTR))
    {
      printf("superblock found at block %d\n", i);
      break;
    }
    i++;
  }


  //get start of inodes

  //inode 0 = rootdir
  printf("rootdir: %d\n", super->rootdir);
  printf("inodes: %d\n", super->inodes);
  char temp[64];
  sfs_inode_t *temp_inode = (sfs_inode_t *)temp;
  driver_read(temp_inode, super->rootdir);
  printf("temp: %d\n", temp_inode->owner);

  
  //printf("inode 0 size: %d\n", inode->direct[1]);
  

  // printf("inode->direct[0]: %d\n", inodes[0].direct[0]);

  // printf("rootdir: %d\n", super->rootdir);
  // printf("inodes: %d\n", super->inodes);

  // for(int j = 0; j < 5; j++){
  //   char raw_dir[128];
  //   sfs_dirent *dir = (sfs_dirent *)raw_dir;
  //   driver_read(dir, inodes[1].direct[j]);
  //   printf("dir %d name: %s\n", j, dir->name);
  //   printf("dir %d: inode: %d\n", j, dir->inode);
  // }


  /* is it the filesystem superblock? */
  // if(super->fsmagic == VMLARIX_SFS_MAGIC &&
  //    ! strcmp(super->fstypestr,VMLARIX_SFS_TYPESTR))
  //   {
  //     printf("superblock found at block 10!\n");
  //   }
  // else
  //   {
  //     /* read block 0 from the disk image */
  //     driver_read(super,0);
  //     if(super->fsmagic == VMLARIX_SFS_MAGIC && !strcmp(super->fstypestr,VMLARIX_SFS_TYPESTR))
  //     {
  //       printf("superblock found at block 0!\n");
  //     }
  //     else
	// printf("superblock is not at block 10 or block 0\nI quit!\n");
  //   }
  
  /* close the disk image */
  driver_detach_disk_image();

  return 0;
}
