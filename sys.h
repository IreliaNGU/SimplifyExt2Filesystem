#ifndef __SYS_H__
#define __SYS_H__

#include "interface.h"
#include "fileop.h"

//打开文件系统,若成功返回0，失败返回-1
int OpenSys();

//关闭文件系统，若成功返回0，失败返回-1
int CloseSys();

//退出文件系统
void shutdown();

//ls
void ls(char* argv[],int argc);

//touch
void touch(char* argv[],int argc);

//mkdir
void mkdir(char* argv[],int argc);

//cp
void cp(char* argv[],int argc);

//shutdown
void shutdown();


#endif