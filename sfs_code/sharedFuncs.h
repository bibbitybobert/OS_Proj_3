#ifndef SHAREDFUNCS_H
#define SHAREDFUNCS_H

#include <driver.h>
#include <sfs_dir.h>
#include <sfs_superblock.h>
#include <sfs_inode.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <bitmap.h>
#include <stdlib.h>
#include <time.h>

void get_file_block(sfs_inode_t n, uint32_t blk_num, char* data);

#endif