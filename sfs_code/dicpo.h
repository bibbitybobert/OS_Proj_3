#ifndef DICPO_H
#define DICPO_H

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
#include <sharedFuncs.h>
#include "sharedFuncs.c"

int main(int argc, char* argv[]);

void read_in_file(char* diskImg, FILE* file, char* fileName);


#endif