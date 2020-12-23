#ifndef __FILEOP_H__
#define __FILEOP_H__
//fileop提供了各种对文件的操作

#include "interface.h"
#include "utils.h"

#define MAGICNUMBER 9525
#define SIZE 1000 //文件大小
#define ROOTINODE 2 //根目录所在索引
#define ROOTBLOCK 2 //根目录所在块
#define NAMESIZE 10 //文件夹以及文件的名字长度

typedef enum type
{
    unknown_type = 0,
    file_type = 1,
    dir_type = 2,
}type_t;

//检查文件系统
void Check_sys();

//初始化超级块
void Init_SPblock();

//初始化inode数组
void Init_inode();

//初始化数据块
void Init_Data();

//初始化根目录
void Init_rootdir();

//寻找并分配一个inode，返回inode的索引值（第几个inode）
int alloc_inode();

//寻找并分配一个block，返回block的索引值（第几个block）
int alloc_block();

//读取根目录下的文件
void Read_Root();

//读取某个路径下的文件
void Read_Path(char* argv);

//根据路径创建文件
void Create_File(char* argv);

//根据路径创建文件夹
void Create_Dir(char* argv);





#endif 