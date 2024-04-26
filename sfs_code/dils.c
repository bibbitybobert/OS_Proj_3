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
        printf("Incorrect number of arguments\n");
        return -1;
    }
    else if(argc == 2){
        fileName = argv[1];
        read_file(fileName);
    }
    else if(argc == 3){
        fileName = argv[1];
        option = argv[2];
        char longOpt[2] = "-l";
        if(!strcmp(option, "-l")){
            read_file_long(fileName);
        }
        else{
            printf("Unknown option\n");
            return -1;
        }
    }
    else{
        printf("Incorrect number of arguments\n");
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
        break;
        }
        i++;
    }

    // printf("Num blocks: %d\n", super->num_blocks);
    // printf("num_inode blocks: %d\n", super->num_inode_blocks);
    // printf("num free inodes: %d\n", super->inodes_free);

    sfs_inode_t inodes[2];
    driver_read(inodes, super->inodes);

    //driver_read(buffer, inodes[0].direct[0]);
    int inodes_per_blk = super->block_size / sizeof(sfs_inode_t);
    int num_read_files = 0;
    int num_files = inodes[0].size / sizeof(sfs_dirent);
    for(int blk = 0; blk < super->num_inode_blocks; blk++){
        char buffer[128];
        get_file_block(inodes[0], blk, buffer);

        sfs_dirent* dir = (sfs_dirent *)buffer;
        int entries_per_block = super->block_size / sizeof(sfs_dirent);

        for(int i = 0; i < entries_per_block; i++){
            if(dir[i].inode >= 0 && num_read_files < num_files){
                num_read_files++;
                printf("%s\n", dir[i].name);
            }
        }
        if(num_files == num_read_files){
            return;
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
    char raw_superblock[128];
    sfs_superblock *super = (sfs_superblock *)raw_superblock;
    driver_attach_disk_image(fileName, 128);

    int i = 0;
    while(i < 128){
        driver_read(super, i);
        if(super->fsmagic == VMLARIX_SFS_MAGIC &&
        ! strcmp(super->fstypestr,VMLARIX_SFS_TYPESTR))
        {
        break;
        }
        i++;
    }

    char raw_root_inode[128];
    driver_read(raw_root_inode, super->inodes);

    sfs_inode_t * inodes = (sfs_inode_t *)raw_root_inode;

    int inodes_per_blk = super->block_size / sizeof(sfs_inode_t);
    int num_read_files = 0;
    int num_files = inodes[0].size / sizeof(sfs_dirent);
    for(int blk = 0; blk < super->num_inode_blocks; blk++){
        char buffer[128];
        get_file_block(inodes[0], blk, buffer);

        sfs_dirent* dir = (sfs_dirent *)buffer;
        int entries_per_block = super->block_size / sizeof(sfs_dirent);

        for(int i = 0; i < entries_per_block; i++){
            if(dir[i].inode >= 0 && num_read_files < num_files){
                num_read_files++;
                char perm_buffer[128];
                driver_read(perm_buffer, super->inodes + (dir[i].inode/2));
                sfs_inode_t* file_perms = (sfs_inode_t *)perm_buffer;

                int which_inode = dir[i].inode%2;
                char perms[11] = {'-','-','-','-','-','-','-','-','-', '-', '\0'};
                set_first_bit(file_perms[which_inode].type, perms);
                get_perms(file_perms[which_inode].perm, perms);
                time_t realtime = (time_t)file_perms[which_inode].atime;
                struct tm* time_tm = localtime(&realtime);

                char time_str[25] = "";
                strftime(time_str, 25, "%c", time_tm);
                printf("%10s %2d %2d %2d %5ld %24s ",
                perms,
                file_perms[which_inode].refcount,
                file_perms[which_inode].owner,
                file_perms[which_inode].group,
                file_perms[which_inode].size,
                time_str);
                printf("%s\n", dir[i].name);

            }
        }
        if(num_files == num_read_files){
            return;
        }
    }

}

void get_perms(uint16_t perms_int, char* perms_char){
    uint16_t mask = 256;
    //char perms[10] = {'-','-', '-', '-','-','-', '-', '-', '-', '-'};
    for(int i = 1; i < 10; i++){
        if(mask & perms_int){
            perms_char[i] = '1';
        }
        mask = mask >> 1;
    }

    if(perms_char[1] == '1'){perms_char[1] = 'r';}
    if(perms_char[2] == '1'){perms_char[2] = 'w';}
    if(perms_char[3] == '1'){perms_char[3] = 'x';}
    if(perms_char[4] == '1'){perms_char[4] = 'r';}
    if(perms_char[5] == '1'){perms_char[5] = 'w';}
    if(perms_char[6] == '1'){perms_char[6] = 'x';}
    if(perms_char[7] == '1'){perms_char[7] = 'r';}
    if(perms_char[8] == '1'){perms_char[8] = 'w';}
    if(perms_char[9] == '1'){perms_char[9] = 'x';}
}

void set_first_bit(uint8_t fileType, char* perms){
    switch(fileType){
        case 0: perms[0] = '-'; break;
        case 1: perms[0] = 'd'; break;
        case 2: perms[0] = 'c'; break;
        case 3: perms[0] = 'b'; break;
        case 4: perms[0] = 'p'; break;
        case 5: perms[0] = 's'; break;
        case 6: perms[0] = 'l'; break;
        default: perms[0] = '-'; break;
    }
}

void get_atime(uint32_t atime, char* dow, char* month, int day, char* time, int year){
    //day of week - month - day - time(military) - year
    char hr[3];
    char min[3];

    struct tm * timeInfo;
    time_t atime_time = atime;
    timeInfo = localtime(&atime_time);

    switch (timeInfo->tm_wday)
    {
        case 0: dow = "Sun";break;
        case 1: dow = "Mon";break;
        case 2: dow = "Tue";break;
        case 3: dow = "Wed";break;
        case 4: dow = "Thu";break;
        case 5: dow = "Fri";break;
        case 6: dow = "Sat";break;
        default: break;
    }

    switch(timeInfo->tm_mon){
        case 0: month = "Jan";break;
        case 1: month = "Feb";break;
        case 2: month = "Mar";break;
        case 3: month = "Apr";break;
        case 4: month = "May";break;
        case 5: month = "Jun";break;
        case 6: month = "Jul";break;
        case 7: month = "Aug";break;
        case 8: month = "Sep";break;
        case 9: month = "Oct";break;
        case 10: month = "Nov";break;
        case 11: month = "Dec";break;
        default: break;
    }

    day = timeInfo->tm_mday;
    sprintf(hr, "%d", timeInfo->tm_hour);
    sprintf(min, "%d", timeInfo->tm_min);
    strcat(time, hr);
    char* colon = ":";
    strcat(time, colon);
    strcat(time, min);
    year = timeInfo->tm_year;
}