#ifndef DILS_H
#define DILS_H

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

int main(int argc, char* argv[]);

void read_file(char* fileName);

void read_file_long(char* fileName);

void get_file_block(sfs_inode_t n, uint32_t blk_num, char* data);

void get_perms(uint16_t perm_int, char* perms);

void get_atime(uint32_t atime, char* dow, char* month, int day, char* time, int year);


#endif