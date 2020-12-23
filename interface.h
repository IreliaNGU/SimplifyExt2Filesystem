#ifndef __INTERFACE_H__
#define __INTERFACE_H__
//interface提供了两个函数，ReadToDisk和WriteToDisk,是对磁盘的接口
#include <stdint.h>
#include "disk.h"
#define MAX_INODE 1024  //inode的总数
#define MAX_INODE_BLOCK 32 //inode数组所占的block数
#define INODE_BYTE 32 //一个inode占32字节
#define INODEMAP 32
#define BLOCKMAP 128
#define BLOCK_BYTE 1024 //一个block占1KB
#define MAX_ITEM_IN_BLOCK 8 //一个block内如果都用dir_item存储能存的最大dir_item数
#define MAX_BLOCK 4096 //整个文件系统的最大块数
#define MAX_DATABLOCK 4063 //数据块的最大块数
#define MAX_SPACE 1024*1024*4//4MB的总大小
#define INODE_START 1024 //inode部分的开始字节
#define DATA_START 33*1024 //DATA部分的开始字节

//超级块(1block)
typedef struct super_block {
    int32_t magic_num;                  // 幻数
    int32_t free_block_count;           // 空闲数据块数
    int32_t free_inode_count;           // 空闲inode数
    int32_t dir_inode_count;            // 目录inode数
    uint32_t block_map[128];            // 数据块占用位图
    uint32_t inode_map[32];             // inode占用位图
} SPblock;


//单个inode
typedef struct inode{
    uint32_t size;              // 文件大小
    uint16_t file_type;         // 文件类型（文件/文件夹）
    uint16_t link;              // 连接数
    uint32_t block_point[6];    // 数据块指针
}INODE;


//目录项结构体
typedef struct dir_item {               // 目录项一个更常见的叫法是 dirent(directory entry)
    uint32_t inode_id;          // 当前目录项表示的文件/目录的对应inode
    uint16_t valid;             // 当前目录项是否有效 
    uint8_t type;               // 当前目录项类型（文件/目录）
    char name[121];             // 目录项表示的文件/目录的文件名/目录名
}DIRITEM;

//一个block，存储文件内容的block也使用这种结构
typedef struct datablock
{
    DIRITEM block_item[MAX_ITEM_IN_BLOCK];
}DATABLOCK;

//文件系统
struct sys
{
    SPblock spblock;
    //inode数组
    //1block内有32个inode,一共有1024个inode，所以一共需要32个block组成inode数组。
    INODE inodes[MAX_INODE];
    DATABLOCK datablocks[MAX_DATABLOCK];
};

//实例化一个文件系统
struct sys mysys;

//缓冲区
char sys_buf[MAX_SPACE]; //DEVICE *2 * MAX_BLOCK

//从磁盘中读取4MB的内容到sys结构体中，成功返回0，失败返回-1
int ReadFromDisk();

//从磁盘中写4MB的内容到sys结构体中，成功返回0，失败返回-1
int WriteToDisk();


#endif
