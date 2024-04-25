#include <dicpo.h>

int main(int argc, char* argv[]){
    char *diskImgName;
    char *fileName;

    if(argc < 3){
        printf("Please input 2 arguments (disk img name, file name)\n");
        return -1;
    }
    else if(argc == 3){
        diskImgName = argv[1];
        fileName = argv[2];
        FILE *fptr = fopen(fileName, "wb");
        read_in_file(diskImgName, fptr, fileName);
        fclose(fptr);
    }
    else{
        printf("Please input 2 arguments (disk img name, file name)\n");
        return -1;
    }
}

void read_in_file(char* disk, FILE* file, char* fileName){
    char raw_superblock[128];
    sfs_superblock *super = (sfs_superblock *)raw_superblock;
    driver_attach_disk_image(disk, 128);

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
                if(!strcmp(dir[i].name, fileName)){
                    char data[128];
                    int start_blk = super->inodes + (dir[i].inode/2);
                    driver_read(data, start_blk);
                    sfs_inode_t* data_inodes = (sfs_inode_t *)data;
                    int which_inode = dir[i].inode%2;
                    char file_content[super->block_size];
                    for(int blk = 0; blk <= (data_inodes[which_inode].size / super->block_size); blk++){
                        get_file_block(data_inodes[which_inode], blk, file_content);
                        if(blk == data_inodes[which_inode].size / super->block_size){
                            int amt_to_write = data_inodes[which_inode].size % super->block_size;
                            fwrite(file_content, amt_to_write, 1, file);
                        }
                        else{
                            fwrite(file_content, 128, 1, file);
                        }
                        // fprintf(file, "%s", file_content);
                        strcpy(file_content, "");
                    }

                }
            }
        }
        if(num_files == num_read_files){
            return;
        }
    }
    driver_detach_disk_image();

}