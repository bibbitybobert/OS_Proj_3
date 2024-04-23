#include <driver.h>
#include <sfs_dir.h>
#include <sfs_superblock.h>
#include <sfs_inode.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dils.h>

int main(int argc, char* argv[]){
    char *fileName;
    char *option;
    if(argc < 2){
        printf("error please try again\n");
        return -1;
    }
    else if(argc == 2){
        fileName = argv[1];
        read_file(fileName);
    }
    else if(argc == 3){
        fileName = argv[1];
        option = argv[2];
        if(option == "-l"){
            read_file_long(fileName);
        }
        else{
            printf("Error please try again\n");
            return -1;
        }
    }
    else{
        printf("Error please try again\n");
        return -1;
    }
    return 0;
    
}

void read_file(char* fileName){
    char raw_superblock[128];
    sfs_superblock *super = (sfs_superblock *)raw_superblock;
    driver_attach_disk_image(fileName, 128);

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


    sfs_inode_t inodes[2];
    driver_read(inodes, super->inodes);

    char buffer[128];

    //driver_read(buffer, inodes[0].direct[0]);
    get_file_block(inodes[0], 0, buffer);

    sfs_dirent* dir = (sfs_dirent *)buffer;
    int entries_per_block = super->block_size / sizeof(sfs_dirent);

    printf("entries per block: %d\n", entries_per_block);

    for(int i = 0; i < entries_per_block; i++){
        if(dir[i].inode >= 0){
            printf("name: %s\n", dir[i].name);
        }
    }

    
    /*
    printing out access time
    day of week - month - day - time(military) - year
    asctime();

    superblock incorrect with rootdir, num free inodes
    correct with inode table, bitmap

    when writing, correct superblock


    */



    driver_detach_disk_image();

}

void read_file_long(char* fileName){

}

void get_file_block(sfs_inode_t n, uint32_t blk_num, char* data){
    uint32_t ptrs[32];
    if(blk_num < 5){
        driver_read(data, n.direct[blk_num]);
    }
    else if(blk_num < 5 + 32){ 
        driver_read(ptrs, n.indirect);
        driver_read(data, ptrs[blk_num-5]);
    }
    else if(blk_num < (5 + 32 + (32*32))){
        driver_read(ptrs, n.dindirect);
        int tmp = (blk_num-5-32)/32;
        tmp = ptrs[tmp];
        driver_read(ptrs, ptrs[tmp]);
        tmp = (blk_num-5-32)%32;
        driver_read(data, ptrs[tmp]);
    }
    else if(blk_num < (5 + 32 + (32*32) + (32*32*32))){
        driver_read(ptrs, n.tindirect);
        //do later
    }
    else{
        printf("error reading in data\n");
        exit(-1);
    }
}