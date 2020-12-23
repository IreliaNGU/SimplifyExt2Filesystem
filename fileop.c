#include <stdio.h>
#include <string.h>
#include "fileop.h"

void Init_SPblock(){
    
    mysys.spblock.magic_num = MAGICNUMBER;

    mysys.spblock.free_block_count = MAX_DATABLOCK;

    mysys.spblock.free_inode_count = MAX_INODE;

    mysys.spblock.dir_inode_count = 0;

    for(int i=0;i<128;i++){
         mysys.spblock.block_map[i]=0;
    }

    for(int i=0;i<32;i++){
        mysys.spblock.inode_map[i]=0;
    }

}

void Init_inode(){
    for(int i=0;i<MAX_INODE;i++){

        //文件大小为自定义
        mysys.inodes[i].size = SIZE;
        mysys.inodes[i].link = 1;
        mysys.inodes[i].file_type = unknown_type;
        for(int j=0;j<6;j++){
            mysys.inodes[i].block_point[j]=0;
        }

    }
}

void Init_Data(){
    for(int i=0;i<MAX_DATABLOCK;i++){
        for(int j=0;j<MAX_ITEM_IN_BLOCK;j++){
            mysys.datablocks[i].block_item[j].inode_id=0;

            //初始化为0，表示可用
            mysys.datablocks[i].block_item[j].valid=0;

            mysys.datablocks[i].block_item[j].type=unknown_type;
        }
    }
}

void Init_rootdir(){
    //根目录在inode中类型为dir
    mysys.inodes[ROOTINODE].file_type=dir_type;

    mysys.inodes[ROOTINODE].block_point[0] = ROOTBLOCK;

    //更改超级块信息
    mysys.spblock.dir_inode_count++;

    mysys.spblock.free_inode_count--;

    mysys.spblock.inode_map[0] |= 0x20000000;
}


void Check_sys(){

    //检查自定义幻数
    if(mysys.spblock.magic_num!=MAGICNUMBER){ 

        //文件系统不存在或已损坏，需要重新建立
        printf("magic number wrong.Rebuilding the file system.");

        Init_SPblock();

        Init_inode();

        Init_Data();

        Init_rootdir();
        return;
    } else{
        //可识别的文件系统
        printf("Correct magic number.Enter the file system.");
        return;
    }
}

int alloc_inode(){
    //检查是否有空inode
    if (mysys.spblock.free_inode_count = 0){
        printf("Not enough inode.");;
        return -1;
    }

    //检查位图获得一个为0的位
    for(int i=0;i<INODEMAP;i++){
        uint32_t inode = mysys.spblock.inode_map[i];
        for (int j = 0; j < 32; j++) {
            if ((inode >> j) & 1) continue;
            else {
                mysys.spblock.free_inode_count--;
                mysys.spblock.inode_map[i] |= 1 << j;
                return i*32 + 31-j;
            }
        }
    }
}

int alloc_block(){
    if(mysys.spblock.free_block_count == 0 ){
        printf("Not enough block.");
        return -1;
    }

    for(int i=0;i<BLOCKMAP;i++){
        uint32_t block = mysys.spblock.block_map[i];
       for (int j = 0; j < 32; j++) {
            if ((block >> j) & 1) continue;

            //避免在block0存东西，以引发与没存之间的矛盾
            if (j==31) continue;
            else {
                mysys.spblock.free_block_count--;
                mysys.spblock.block_map[i] |= 1 << j;
                return i*32 + 31-j;
            }
        }
    }
}

void Read_Root(){

    INODE* p_root = &mysys.inodes[ROOTINODE];

    //根目录指向的块
    int index_rootblock = p_root->block_point[0];
    if(index_rootblock==0){ //根目录下还没有存文件
        printf("No file/dir yet.\n");
        return;
    }else{
        //遍历一个block之内的8个dir_item,寻找已被使用的item
        for(int i=0; i<MAX_ITEM_IN_BLOCK;i++){

            if(mysys.datablocks[index_rootblock].block_item[i].valid==1){

                switch (mysys.datablocks[index_rootblock].block_item[i].type)
                {
                case file_type:
                    printf("Name: %s    Type: file\n",mysys.datablocks[index_rootblock].block_item[i].name);
                    break;
                case dir_type:
                    printf("Name: %s    Type: dir\n",mysys.datablocks[index_rootblock].block_item[i].name);
                default:
                    break;
                }

            }
        }
        
    }
    
    return;
}

void Read_Path(char* argv){

    char name[NAMESIZE];
    init_buf(name,NAMESIZE);

    //层数
    int num = get_path_and_name(argv,name);

    INODE* p_root = &mysys.inodes[ROOTINODE];

    //根目录指向的块
    int index_rootblock = p_root->block_point[0];

    if(index_rootblock==0){ //根目录下还没有存文件
        printf("No file/dir yet.\n");
        return;
    }


    DATABLOCK temp_block = mysys.datablocks[index_rootblock];

    int temp_inodeid;

    int temp_blockid;

    //根据层数沿着路径找到最后一个目录下的文件
    for(int i=0;i<num;i++){
        int j=0;
        //找block下的每一个dir_item
        for(;j<MAX_ITEM_IN_BLOCK;j++){

            //找到当前block中与对应参数相同的dir_item
            if(temp_block.block_item[j].valid && !strcmp(cur_path[i],temp_block.block_item[j].name)){

                //获取下一路径的inode id
                temp_inodeid = temp_block.block_item[j].inode_id;

                break;
            }

        }

        if(j==MAX_ITEM_IN_BLOCK){
            printf("No such file or directory.\n");
            return;
        }

        //获取该inode id所对应的block
        temp_blockid = mysys.inodes[temp_inodeid].block_point[0];

        //该block中没有文件
        if(temp_blockid==0){
            printf("Nothing in directory %s.\n",temp_block.block_item[j].name);
            return;
        }

        //更新temp_block为下一路径的block
        temp_block = mysys.datablocks[temp_blockid];
    }

    if(mysys.inodes[temp_inodeid].file_type == file_type){
        printf("This is a file.Please input a directory.\n");
    }

    //输出temp_block的内容
    //遍历一个block之内的8个dir_item,寻找已被使用的item
    for(int i=0; i<MAX_ITEM_IN_BLOCK;i++){

        //只输出valid为1并且为文件夹类型的文件
        if(temp_block.block_item[i].valid==1){


            switch (temp_block.block_item[i].type)
            {
            case file_type:
                printf("Name: %s    Type: file\n",temp_block.block_item[i].name);
                break;
            case dir_type:
                printf("Name: %s    Type: dir\n",temp_block.block_item[i].name);
            default:
                break;
            }

        }
    }
    
    return;
}

void Create_File(char* argv){
    char name[NAMESIZE];
    init_buf(name,NAMESIZE);
    
    //层数
    int num = get_path_and_name(argv,name);

    INODE* p_root = &mysys.inodes[ROOTINODE];

    //根目录指向的块
    int index_rootblock = p_root->block_point[0];


    DATABLOCK temp_block = mysys.datablocks[index_rootblock];

    int temp_inodeid = ROOTINODE;

    int temp_blockid = ROOTBLOCK;

    //根据层数沿着路径找到最后一个目录下的文件
    //注意最后一个是要创建的文件，所以与上面不同只用找到倒数第二个路径
    for(int i=0;i< num-1 ;i++){
        int j=0;
        //找block下的每一个dir_item
        for(;j<MAX_ITEM_IN_BLOCK;j++){

            //找到当前block中与对应参数相同的dir_item
            if(temp_block.block_item[j].valid && !strcmp(cur_path[i],temp_block.block_item[j].name)){

                //获取下一路径的inode id
                temp_inodeid = temp_block.block_item[j].inode_id;

                break;
            }

        }

        if(j==MAX_ITEM_IN_BLOCK){
            printf("No such file or directory.\n");
            return;
        }

        //获取该inode id所对应的block
        temp_blockid = mysys.inodes[temp_inodeid].block_point[0];

        //该block中没有文件
        if(temp_blockid==0){
            printf("Nothing in directory %s.\n",temp_block.block_item[j].name);
            return;
        }

        //更新temp_block为下一路径的block
        temp_block = mysys.datablocks[temp_blockid];
    }

    int newinodeid;
    int newblockid;
    for(int i=0;i<MAX_ITEM_IN_BLOCK;i++){
        //可用的dir_item

        if(temp_block.block_item[i].valid==0){


            mysys.datablocks[temp_blockid].block_item[i].valid =1;

            mysys.datablocks[temp_blockid].block_item[i].type=file_type;

            strcpy(mysys.datablocks[temp_blockid].block_item[i].name,name);
            // mysys.datablocks[temp_blockid].block_item[i].name= cur_path[num-1];

            //分配新的inode
            if((newinodeid = alloc_inode())!=-1){
                mysys.datablocks[temp_blockid].block_item[i].inode_id=newinodeid;
            }else{
                printf("Fail to create file.\n");
                return;
            }

            //为新的inode分配新的block
            if((newblockid = alloc_block())!=-1){
                mysys.inodes[newinodeid].block_point[0] = newblockid;
            }

            mysys.inodes[newinodeid].file_type = file_type;

            printf("Successfully create a file.\n");

            return;
        }
    }
}

void Create_Dir(char* argv){
    char name[NAMESIZE];
    init_buf(name,NAMESIZE);
    
    //层数
    int num = get_path_and_name(argv,name);

    INODE* p_root = &mysys.inodes[ROOTINODE];

    //根目录指向的块
    int index_rootblock = p_root->block_point[0];


    DATABLOCK temp_block = mysys.datablocks[index_rootblock];

    int temp_inodeid = ROOTINODE;

    int temp_blockid = ROOTBLOCK;

    //根据层数沿着路径找到最后一个目录下的文件
    //注意最后一个是要创建的文件，所以与上面不同只用找到倒数第二个路径
    for(int i=0;i< num-1 ;i++){
        int j=0;
        //找block下的每一个dir_item
        for(;j<MAX_ITEM_IN_BLOCK;j++){

            //找到当前block中与对应参数相同的dir_item
            if(temp_block.block_item[j].valid && !strcmp(cur_path[i],temp_block.block_item[j].name)){

                //获取下一路径的inode id
                temp_inodeid = temp_block.block_item[j].inode_id;

                break;
            }

        }

        if(j==MAX_ITEM_IN_BLOCK){
            printf("No such file or directory.\n");
            return;
        }

        //获取该inode id所对应的block
        temp_blockid = mysys.inodes[temp_inodeid].block_point[0];

        //该block中没有文件
        if(temp_blockid==0){
            printf("Nothing in directory %s.\n",temp_block.block_item[j].name);
            return;
        }

        //更新temp_block为下一路径的block
        temp_block = mysys.datablocks[temp_blockid];
    }

    int newinodeid;
    int newblockid;
    for(int i=0;i<MAX_ITEM_IN_BLOCK;i++){
        //可用的dir_item

        if(temp_block.block_item[i].valid==0){


            mysys.datablocks[temp_blockid].block_item[i].valid =1;

            mysys.datablocks[temp_blockid].block_item[i].type=dir_type;

            strcpy(mysys.datablocks[temp_blockid].block_item[i].name,name);
            // mysys.datablocks[temp_blockid].block_item[i].name= cur_path[num-1];

            //分配新的inode
            if((newinodeid = alloc_inode())!=-1){
                mysys.datablocks[temp_blockid].block_item[i].inode_id=newinodeid;
            }else{
                printf("Fail to create dir.\n");
                return;
            }

            //为新的inode分配新的block
            if((newblockid = alloc_block())!=-1){
                mysys.inodes[newinodeid].block_point[0] = newblockid;
            }

            mysys.inodes[newinodeid].file_type = dir_type;

            printf("Successfully create a directory.\n");

            return;
        }
    }
}

void Copy(char* argv1,char* argv2){

    char name[NAMESIZE];
    init_buf(name,NAMESIZE);
    
    //argv2层数
    int num = get_path_and_name(argv2,name);

    INODE* p_root = &mysys.inodes[ROOTINODE];

    //根目录指向的块
    int index_rootblock = p_root->block_point[0];


    DATABLOCK temp_block = mysys.datablocks[index_rootblock];

    int temp_inodeid = ROOTINODE;

    int temp_blockid = ROOTBLOCK;

    //根据层数沿着路径找到最后一个目录下的文件
    for(int i=0;i< num ;i++){
        int j=0;
        //找block下的每一个dir_item
        for(;j<MAX_ITEM_IN_BLOCK;j++){

            //找到当前block中与对应参数相同的dir_item
            if(temp_block.block_item[j].valid && !strcmp(cur_path[i],temp_block.block_item[j].name)){

                //获取下一路径的inode id
                temp_inodeid = temp_block.block_item[j].inode_id;

                break;
            }

        }

        if(j==MAX_ITEM_IN_BLOCK){
            printf("No such file or directory.\n");
            return;
        }

        //获取该inode id所对应的block
        temp_blockid = mysys.inodes[temp_inodeid].block_point[0];

        //该block中没有文件
        if(temp_blockid==0){
            printf("Nothing in directory %s.\n",temp_block.block_item[j].name);
            return;
        }

        //更新temp_block为下一路径的block
        temp_block = mysys.datablocks[temp_blockid];
    }


    
    //argv1层数
    int num2 = get_path_and_name(argv1,name);

    DATABLOCK temp_block2 = mysys.datablocks[index_rootblock];

    int temp_inodeid2 = ROOTINODE;

    int temp_blockid2 = ROOTBLOCK;

    //根据层数沿着路径找到最后一个目录下的文件
    //注意最后一个是要创建的文件，所以与上面不同只用找到倒数第二个路径
    for(int i=0;i< num2-1 ;i++){
        int j=0;
        //找block下的每一个dir_item
        for(;j<MAX_ITEM_IN_BLOCK;j++){

            //找到当前block中与对应参数相同的dir_item
            if(temp_block2.block_item[j].valid && !strcmp(cur_path[i],temp_block2.block_item[j].name)){

                //获取下一路径的inode id
                temp_inodeid2 = temp_block2.block_item[j].inode_id;

                break;
            }

        }

        if(j==MAX_ITEM_IN_BLOCK){
            printf("No such file or directory.\n");
            return;
        }

        //获取该inode id所对应的block
        temp_blockid2 = mysys.inodes[temp_inodeid2].block_point[0];

        //该block中没有文件
        if(temp_blockid2==0){
            printf("Nothing in directory %s.\n",temp_block2.block_item[j].name);
            return;
        }

        //更新temp_block为下一路径的block
        temp_block2 = mysys.datablocks[temp_blockid2];
    }

    int newinodeid;
    int newblockid;
    for(int i=0;i<MAX_ITEM_IN_BLOCK;i++){
        //可用的dir_item

        if(temp_block2.block_item[i].valid==0){


            mysys.datablocks[temp_blockid2].block_item[i].valid =1;

            mysys.datablocks[temp_blockid2].block_item[i].type=file_type;

            strcpy(mysys.datablocks[temp_blockid2].block_item[i].name,name);
            // mysys.datablocks[temp_blockid].block_item[i].name= cur_path[num-1];

            //分配新的inode
            if((newinodeid = alloc_inode())!=-1){
                mysys.datablocks[temp_blockid2].block_item[i].inode_id=newinodeid;
            }else{
                printf("Fail to create file.\n");
                return;
            }

            //为新的inode分配新的block
            if((newblockid = alloc_block())!=-1){
                mysys.inodes[newinodeid].block_point[0] = newblockid;
            }

            mysys.inodes[newinodeid].file_type = file_type;

            //将argv2的block的内容复制到argv1中
            for(int i=0;i<MAX_ITEM_IN_BLOCK;i++){
                mysys.datablocks[newblockid].block_item[i] = temp_block.block_item[i];
            }

            printf("Successfully copy a file.\n");

            return;
        }
    }
}
