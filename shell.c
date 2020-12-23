#include <stdio.h>
#include "shell.h"
#include "disk.h"
#include "fileop.h"
#include "utils.h"
#include <ctype.h>

int main(){
    if(OpenSys()==-1){ //没有成功打开文件系统
        printf("Fail to open file system.\n");
        shutdown();
    }

    printf("Welcome to hzl's file system!\n");

    while (1)
    {
        printf("Lai's ");

        //读取输入到缓冲区
        fgets(buf,SHELL_LEN,stdin);

        //将缓冲区中的输入分析提取到argv与argc中
        split();

        //根据argv与argc执行指令
        exec();
        
    }
    
}

void split(){

    char* word = buf;
    int i=0; //参数的数量

    //跳过前面的空格
    word = skip_space(word);

    //定位到第一个空格处
    char* space = strchr(word,' ');

    while(space!=NULL){
        //分隔开第一个单词和后面的字符串
        space[0] = '\0';

        //将分隔得到的单词赋值给参数数组
        argv[i] = word;

        //从该空格后的下一个字符继续跳过空格得到新的未处理输入
        word = skip_space(space+1);

        i++; //准备接收下一个参数

        //寻找下一个空格
        space = strchr(word,' ');
    }

    //处理最后一个字符
    if(word[0]!='\0'){
        argv[i] = word;
        space=strchr(word,'\n');
        space[0] = '\0';
        i++;
    }

    
    argv[i] = NULL;

    //参数数量
    argc = i;
}

void exec(){

    char* command = argv[0];
    //ls指令
    if(!strcmp(command,"ls")){
        ls(argv,argc);
    }else if(!strcmp(command,"touch")){
        touch(argv,argc);
    }else if(!strcmp(command,"mkdir")){
        mkdir(argv,argc);
    }else if(!strcmp(command,"cp")){
        cp(argv,argc);
    }else if(!strcmp(command,"shutdown")) {
        shutdown();
    }else{
        printf("Invalid command.\n");
    }
    
}