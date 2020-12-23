#ifndef SHELL_H
#define SHELL_H

#include "sys.h"
#include "utils.h"
#include <string.h>

#define SHELL_LEN 32 //shell 一次输入的长度

//shell输入的缓冲区
char buf[SHELL_LEN];

//参数值
char* argv[SHELL_LEN];

//参数数量
int argc=0;


//分析提取输入缓冲区中的信息,将指令与路径分离开
void split();

//执行指令
void exec();



#endif