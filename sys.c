#include "sys.h"
#include <stdio.h>
#include <stdlib.h>

int OpenSys(){
    if(ReadFromDisk()==-1){
        printf("ReadFromDisk fail.\n");
        return -1;
    }

    Check_sys();

    return 0;
}

int CloseSys(){

    //写回磁盘失败
    if(WriteToDisk()==-1){
        printf("CloseSys: WriteToDisk fail.\n");
        return -1;
    }

    return 0;
}

void shutdown(){
    if(CloseSys()==-1){
        printf("shutdown: CloseSys fail.\n");
        exit(0);
    }
    
    printf("Successfully shutdown.\n");
    exit(0);
}

void ls(char* argv[],int argc){
    if(argc > 2){
        printf("Too many arguments.\n");
    }else if(argc == 1){  //输出根目录下的文件
        Read_Root();
    }else if(argc ==2){ //输出某路径下的内容
        Read_Path(argv[1]);
    }
}

void touch(char* argv[],int argc){
    if (argc > 2){
        printf("Too many arguments.\n");
    }else if(argc == 1){
        printf("Just receive 1 argument.\n");
    }else if(argc ==2){
        Create_File(argv[1]);
    }else
    {
        printf("Something strange about argument wrong?");
    }
    
}

void mkdir(char* argv[],int argc){
   if (argc > 2){
        printf("Too many arguments.\n");
    }else if(argc == 1){
        printf("Just receive 1 argument.\n");
    }else if(argc ==2){
        Create_Dir(argv[1]);
    }else
    {
        printf("Something strange about argument wrong\n");
    }
}

void cp(char* argv[],int argc){
    if(argc > 3){
        printf("Too many arguments.\n");
    }else if(argc < 3){
        printf("Too few arguements.\n");
    }else if(argc == 3 ){
        Copy(argv[1],argv[2]);
    }

}

