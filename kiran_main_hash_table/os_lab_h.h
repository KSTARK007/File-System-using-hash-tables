#ifndef File_system_HEADER_H
#define File_system_HEADER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef char BLK_DATA_TYPE;

#define NUM_INODES 100
#define NUM_DATA_ELEMS 4096
#define NUM_BLKS 1000
#define BLK_SIZE sizeof(block)
#define INODE_SIZE sizeof(inode)
#define MAX_PATH_LEN 252
#define MAX_CHILDREN 10
#define MAX_LEVEL 5
#define FREE_BLK 0
#define STACK_TOP 0
#define INODES_LIST 0 
#define INODE_OFFSET 0 
#define DATA_OFFSET (INODE_OFFSET + sizeof(struct inode)*NUM_INODES)
#define FILE_NAME "./memory_file.dat"

int init_storage();

/*------------------------------------- FILE DESCRIPTOR ----------------------------------------*/
FILE *mem_fil;

/*------------------------------------- STRUCTURES ---------------------------------------------*/
typedef struct block
{
    BLK_DATA_TYPE data[NUM_DATA_ELEMS];
    long next;
}block;

typedef struct inode
{

    char name[50];
    int inode_ID;//starts from 0 to 99 for phase 1
    int is_dir;
    int inode_path[MAX_LEVEL];
    int parent;//_inode
    int children_inodes[MAX_CHILDREN];
    int file_size;
    int file_number_of_blocks;
    int file_first_head;
}inode;


/*typedef struct inode_list
{
    int inode_ID;
    char name[50];
    int inode_path[5];
}inode_list;*/

/*------------------------------------ METHOD PROTOTYPES -------------------------------------*/

int init_storage();
int get_free_block();
int free_blks;//number_of_free_data_blocks
int free_blk_number;//index of the datablock which is free
long HashTable[NUM_BLKS]; 
struct inode *root;
struct inode *find_path(char *path, int is_dir);
struct inode *child_exists(struct inode *parent, char *child);
struct inode *createChild(struct inode *parent, char *child, int is_dir);
void write_free_blk_disk(long new_val);
int read_disk_block(long offset, block *buf);
int write_disk_block(long offset, block *buf);
int read_disk_inode(long offset, inode *buf);
int write_disk_inode(inode *buf);
#endif //File_system_HEADER_H
