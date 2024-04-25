#include <sharedFuncs.h>

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
        driver_read(ptrs, ptrs[tmp]);
        tmp = (blk_num-5-32)%32;
        driver_read(data, ptrs[tmp]);
    }
    else if(blk_num < (5 + 32 + (32*32) + (32*32*32))){
        driver_read(ptrs, n.tindirect);
        int id_of_blk = blk_num-5-32-(32*32);
        int tmp = id_of_blk/(32 * 32);
        driver_read(ptrs, ptrs[tmp]);//double indirect now
        id_of_blk = id_of_blk - (tmp * (32*32));
        tmp = id_of_blk / 32;
        driver_read(ptrs, ptrs[tmp]);
        tmp = tmp % 32;
        driver_read(data, ptrs[tmp]);
    }
    else{
        printf("error reading in data\n");
        exit(-1);
    }
}