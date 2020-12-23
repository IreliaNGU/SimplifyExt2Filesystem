#include <stdio.h>
#include "interface.h"


int ReadFromDisk(){

    //打开磁盘
    if(open_disk()==-1){
        printf("Fail to open the disk.");
        return -1;
    }

    //读入超级块

    //读入超级块前半个Block
    if(disk_read_block(0,&sys_buf[0])==-1){
        printf("Fail to read spblock1 from the disk.");
        return -1;
    }
    //读入超级块后半个Block
    if(disk_read_block(1,&sys_buf[DEVICE_BLOCK_SIZE])==-1){
        printf("Fail to read spblock2 from the disk.");
        return -1;
    }

    //定义一个指针指向超级块
    char *spb_pointer = (char *)&mysys.spblock;

    //将缓冲区中的内容传递给超级块
    for(int i=0;i<sizeof(SPblock);i++){
        spb_pointer[i] = sys_buf[i];
    }


    //读入inode数组到缓冲区
    for(int i=0;i<MAX_INODE_BLOCK*2;i++){
        if(disk_read_block(i+2,&sys_buf[DEVICE_BLOCK_SIZE*(i+2)])==-1){
            printf("Fail to read inode block %d from the disk.",(i+2)/2+1);
            return -1;
        }
    }
    
    //定义一个指针指向inode数组
    char *inode_pointer = NULL;

    //将缓冲区中的内容传递给inode数组
    for(int i=0;i<MAX_INODE;i++){

        //指向当前数组元素
        inode_pointer=(char *)&mysys.inodes[i];

        //将缓冲区中的内容传递给inode数组当前inode
        for(int j=0;j<sizeof(INODE);j++){
            inode_pointer[j] = sys_buf[INODE_START+i*INODE_BYTE+j];
        }
    }

    //读入datablock到缓冲区
    for(int i=0;i<MAX_DATABLOCK*2;i++){
        if(disk_read_block(i+(1+MAX_INODE_BLOCK)*2,&sys_buf[DEVICE_BLOCK_SIZE*(i+(1+MAX_INODE_BLOCK)*2)])==-1){
            printf("Fail to read data block %d from the disk.",(i+(1+MAX_INODE_BLOCK)*2)/2+1);
            return -1;
        }
    }

    //定义一个指针指向数据块数组
    char *data_pointer = NULL;

    //将缓冲区中的内容传递给数组
    for(int i=0;i<MAX_DATABLOCK;i++){

        //指向当前数组元素
        data_pointer=(char *)&mysys.datablocks[i];

        //将缓冲区中的内容传递给datablock数组当前block
        for(int j=0;j<sizeof(DATABLOCK);j++){
            data_pointer[j] = sys_buf[DATA_START+i*BLOCK_BYTE+j];
        }
    }

    //关闭磁盘
    if(close_disk()==-1){
        printf("Readfromdisk:close_disk fail.");
        return -1;
    }

    return 0;
}

int WriteToDisk(){

    //打开磁盘
    if(open_disk()==-1){
        printf("Fail to open the disk.");
        return -1;
    }

     //定义一个指针指向超级块
    char *spb_pointer = (char *)&mysys.spblock;

    //将缓冲区中的内容传递给超级块
    for(int i=0;i<sizeof(SPblock);i++){
        sys_buf[i] = spb_pointer[i];
    }

    if (disk_write_block(0, &sys_buf[0]) < 0){
        printf("Fail to write super block1 to the disk.");
        return -1;
    }
        
    if (disk_write_block(1, &sys_buf[DEVICE_BLOCK_SIZE]) < 0){
        printf("Fail to write super block2 to the disk.");
        return -1;
    }

    //定义一个指针指向inode数组
    char *inode_pointer = NULL;

    //将inode数组的内容传递给缓冲区
    for(int i=0;i<MAX_INODE;i++){

        //指向当前数组元素
        inode_pointer=(char *)&mysys.inodes[i];

        //将当前inode的内容传递给缓冲区
        for(int j=0;j<sizeof(INODE);j++){
            sys_buf[INODE_START+i*INODE_BYTE+j] = inode_pointer[j];
        }
    }

    //写入缓冲区到disk
    for(int i=0;i<MAX_INODE_BLOCK*2;i++){
        if(disk_write_block(i+2,&sys_buf[DEVICE_BLOCK_SIZE*(i+2)])==-1){
            printf("Fail to write inode block %d from the disk.",(i+2)/2+1);
            return -1;
        }
    }

    //定义一个指针指向数据块数组
    char *data_pointer = NULL;

    //将数组传递给缓冲区
    for(int i=0;i<MAX_DATABLOCK;i++){

        //指向当前数组元素
        data_pointer=(char *)&mysys.datablocks[i];

        //将datablock数组当前block传递给缓冲区
        for(int j=0;j<sizeof(DATABLOCK);j++){
            sys_buf[DATA_START+i*BLOCK_BYTE+j] = data_pointer[j];
        }
    }

    //写入缓冲区到disk
    for(int i=0;i<MAX_DATABLOCK*2;i++){
        if(disk_write_block(i+(1+MAX_INODE_BLOCK)*2,&sys_buf[DEVICE_BLOCK_SIZE*(i+(1+MAX_INODE_BLOCK)*2)])==-1){
            printf("Fail to write data block %d from the disk.",(i+(1+MAX_INODE_BLOCK)*2)/2+1);
            return -1;
        }
    }

    //关闭磁盘
    if(close_disk()==-1){
        printf("WriteToDisk:close disk fail");
        return -1;
    }

    return 0;
}


