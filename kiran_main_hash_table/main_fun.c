#include "os_lab_h.h"

int write_disk_inode(struct inode *buf)
{
    // working with the assumption that sizeof buff is size of data block, managed that in read and write!
    // seek to offset
    long offset = buf->inode_ID;  
    offset *= sizeof(struct inode);
    if(fseek(mem_fil,offset+INODE_OFFSET,SEEK_SET) != 0)
    {
        printf("fseek error in write_disk_block! \n");
        return -1;
    }

    // write it back
    if( fwrite(buf, sizeof(struct inode), 1, mem_fil) != 1)
    {
        printf("fwrite error in write_disk_block! \n");
        return -1;

    }

    printf("write_disk_inode: inode %s write success\n", buf->name);
    return 1; // write success!
}

int read_disk_inode(long offset, struct inode *buf)
{
    // seek to offset
    if( fseek(mem_fil,offset,SEEK_SET) != 0)
    {
        printf("fseek error in read_disk_block! \n");
        return -1;
    }
    if( fread(buf, sizeof(struct inode), 1, mem_fil) != 1)
    {   
        printf("fread error in read_disk_block! \n");
        return -1;

    }

    return 1; // read success!
}






int write_disk_block(long offset, block *buf)
{
    printf("write disk block called! %ld\n",offset);    

    // seek to offset
    if( fseek(mem_fil,offset,SEEK_SET) != 0)
    {
        printf("fseek error in write_disk_block! \n");
        return -1;
    }

    // write it back
    if( fwrite(buf, sizeof(block), 1, mem_fil) != 1)
    {
        printf("fwrite error in write_disk_block! \n");
        return -1;
    
    }    
    return 1; // write success!
}

int read_disk_block(long offset, block *buf)
{
    printf("read disk block called! %ld\n",offset);
    // seek to offset
    if( fseek(mem_fil,offset,SEEK_SET) != 0)
    {
        printf("fseek error in read_disk_block! \n");
        return -1;
    }
    // read the struct
    if( fread(buf, sizeof(block), 1, mem_fil) != 1)
    {
        printf("fread error in read_disk_block! \n");
        return -1;
    
    }
    return 1; // read success!
}



int update_inodes_list(int option, long *buf) {

    int top;

    if( fseek(mem_fil,INODES_LIST+(sizeof(long)*top),SEEK_SET) != 0)
    {
        printf("fseek error in read_disk_block! \n");
        return -1;
    }
    printf("Update called\n");

    if( option == 1){
        if(fread(buf, sizeof(long), 1, mem_fil) != 1) {
            printf("fread error in read_disk_block! \n");
            return -1;
        }
        else {
            //fwrite top--
            top--;
            if( fseek(mem_fil,STACK_TOP,SEEK_SET) != 0)
            {
                printf("fseek error in read_disk_block! \n");
                return -1;
            }
            if( fwrite(&top, sizeof(int), 1, mem_fil) != 1)
            {
                printf("fread error in read_disk_block! \n");
                return -1;
            }

        }
    }
    else {

        if( fseek(mem_fil,STACK_TOP,SEEK_SET) != 0)
        {
            printf("fseek error in write_disk_block! \n");
            return -1;
        }

        printf("Update called\n");
        top++;
        if( fwrite(&top, sizeof(int), 1, mem_fil) != 1)
        {
            printf("fwrite error in write_disk_block! \n");
            return -1;

        }

        printf("Update called\n");
        if( fseek(mem_fil,INODES_LIST+(sizeof(long)*top),SEEK_SET) != 0)
        {
            printf("fseek error in write_disk_block! \n");
            return -1;
        }
        printf("Update calledssssss\n");
        if( fwrite(buf, sizeof(long), 1, mem_fil) != 1)
        {
            printf("fwrite error in write_disk_block! \n");
            return -1;

        }
        printf("Update calledssssss\n");

    }
    printf("ssssssUnlink calledsfsdfsdd!\n");
}




int get_free_block()
{
    printf("get free block called! free_blk: %d \n",free_blks);    
    if (free_blks == -1 )
    {
        printf("Couldn't find free blocks! in get_free_block");
        return -1;
    }

    block *data_block = malloc(sizeof(block));
    
    int ret_val = free_blks;
    read_disk_block(free_blks, data_block);      
    // update the new free block head to it's next    
    free_blks = data_block->next;
    fseek(mem_fil,0,SEEK_SET);
    fprintf(mem_fil, "%d",free_blks);
    // set the new block's next to -1, reuses the data_block 
    data_block->next = -1;
    write_disk_block(ret_val,data_block); // setting the new block's next to -1
    free(data_block);
    
    fseek(mem_fil,0,SEEK_SET);
    long temp;    
    fscanf(mem_fil, "%ld",&temp);
    printf("Free_blks on disk: %ld",temp); 

    return ret_val;
}


int init_storage()
{
    FILE *file_pointer;
    /*------------------------ DATA SECTION ---------------------------------------------*/
    mem_fil = fopen(FILE_NAME, "w+");
    if(mem_fil == NULL){
        printf("memory_file creation ERROR! \n\n");
        return -1;
    }

    free_blks = DATA_OFFSET; // all blocks are free
    free_blk_number = 100;
    fprintf(mem_fil, "%d", free_blks);
    
    printf("Initializing storage.. \n\n");
    int i = 0; //local counter
    

    /*------------------------hashTable predefination---------------------------------------------*/
    for(i=0;i<1000;i++)
    {
        if(i<100){
            HashTable[i] = -2;//meaning it is a inode
            continue;
        }
        HashTable[i] = -1;//-1 means empty 0= termination other = nextindex
    }
    printf("hashTable init DONE \n\n");

    

    /*----------------------------Inode population-----------------------------------------*/
    
    int file_status = fseek(mem_fil, INODE_OFFSET, SEEK_SET); // seek to the start of the inode section
    
    if(file_status != 0)
    {
        printf("Could not Seek inodes , in init! \n\n");
        return -1;
    }

    printf("Populating inode block..... \n\n");

    struct inode fil_inode;
    int write_status = 0; // status flag

    for(i = 0;i<NUM_INODES;i++)
    {
        fil_inode.inode_ID = i;
        write_status = fwrite(&fil_inode,sizeof(fil_inode), 1, mem_fil);
        if(write_status != 1)
        {
            printf("fwrite failed! in init \n\n");
            return -1;
        }
    }
    printf("Inode Creation and population DONE \n\n");




    /*--------------------------DataBlock Filling---------------------------------------------*/
    file_status = fseek(mem_fil,DATA_OFFSET,SEEK_SET);
    block fil_block;
    write_status = 0; // status flag
    for(i;i<NUM_BLKS;i++)
    {
        fil_block.next = ftell(mem_fil) + sizeof(fil_block);
        write_status = fwrite(&fil_block,sizeof(fil_block),1, mem_fil);
        if(write_status != 1)
        {
            printf("fwrite failed! in init \n\n");
            return -1;
        }
    }
    printf("Data Node Creation and population DONE \n\n");



    printf("Root Creation STARTED \n\n");
    root = (struct inode *)malloc(sizeof(struct inode));

    // make root directory
    strcpy(root->name, "/");
    root->inode_ID = 0;
    root->is_dir = 1;
    root->parent = -1;
    root->file_size = 0;
    root->file_first_head = 0;
    root->file_number_of_blocks = 0;
    for(i=0;i<MAX_CHILDREN;i++)
    {
        root->children_inodes[i] = -1;
    }
    printf("Root Creation DONE \n\n");
    //end of root creation 
    
    write_disk_inode(root);
    printf("Checking for inode init....%d\n\n", INODE_OFFSET);

    struct inode *temp = (struct inode *)malloc(sizeof(struct inode));

    read_disk_inode(INODE_OFFSET, temp);
    printf("Checking for inode init ....Read inode from disk\n\n");

    printf("Init done! \n\n");
    return 1;
}


struct inode *resolve_path(char *path, int is_dir) {

    char *string,*found;
    char *dir_array[MAX_LEVEL];
    string = strdup(path);

    printf("init path: %s\n", path);


    /*Split path by '/' and store in an array*/
    int i = -2; //-2 because strsep returns two garbage values before actual inodes
    while( (found = strsep(&string,"/")) != NULL ) {
        i++;
        dir_array[i] = found;
    }
    for(int k = 0; k <= i; k++) {
        printf("Path %d: %s\n", k, dir_array[k]);
    }
    printf("2.. Added all to array\n");

    if(i == 0) {
        inode *t = child_exists(root, dir_array[0]);
        if(t != NULL) return t;//For getattr
        else if(is_dir != 2) return createChild(root, dir_array[0], is_dir);
        else return NULL;
    }


    //Get to the deepest directory in the path. Assumes child_exists works
    struct inode *temp = (struct inode*)malloc(sizeof(struct inode));
    read_disk_inode(INODE_OFFSET, temp);
    int j;

    for(j = 0; j < i; j++) {
        if((temp = child_exists(temp, dir_array[j])) == NULL) {
            printf("Directory %s does not exist\n", dir_array[j]);
            return NULL;
        }
    }

    printf("j: %d d: %s\n", j, dir_array[1]);

    printf("Traversed to last directory\n");

    struct inode *temp1 = temp;
    if((temp = child_exists(temp, dir_array[j])) == NULL){
        printf("Checking last entry: IF\n");
        if(is_dir != 2) return createChild(temp1, dir_array[j], is_dir);
        return NULL;
    }
    else {
        printf("Checking last entry: ELSE temp: %s\n", temp->name);
        return temp;
    }
}
