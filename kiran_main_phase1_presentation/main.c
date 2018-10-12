#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "os_lab_h.h"


extern struct inode *root;
extern long free_inodes_list[100];
void clrscr()
{
    system("clear");
}

static int file_mkdir(const char *path_name, mode_t mode)
{
    // -- printf("ROOT : %s\n", root->name);
    clrscr();
    char *path = path_name;
    struct inode *temp = resolve_path(path, 1);
    printf("NEW DIR NAME : %s\n", temp->name);
    return 0;

}

static int file_getattr(const char *path, struct stat *stbuf) {
    int res = 0;
    // clrscr();
    // -- printf("file_GETATTR IS CALLED");
    memset(stbuf, 0, sizeof(struct stat));
    // file access modes, is wrong have  to fix
    // change all this
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;

    } else if ((strcmp(path, "..") != 0) && (strcmp(path, ".") != 0)) {

        char *string;
        string = strdup(path);
            struct inode *temp;// = root;
            temp = resolve_path(string, 2);
            if (temp == NULL) {
                // -- printf("Resolve path in getattr did not work\n");
                res = -ENOENT;
            } else {
                // -- printf("temp: %s\n", temp->name);
                stbuf->st_mode = (temp->is_dir == 1)?(S_IFDIR| 0755):(S_IFREG | 0444);
                stbuf->st_nlink = temp->st_nlink;
                stbuf->st_size = temp->st_size;
                stbuf->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
                stbuf->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
                stbuf->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
                stbuf->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now
                stbuf->st_blocks = temp->st_blocks;           
         }

    }
    else
        res = -ENOENT;
    return res;
    
}

static int file_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{

    // -- printf("file_READDIR IS CALLED");
    (void) offset;
    (void) fi;
    // clrscr();
    read_disk_inode(INODE_OFFSET, root);

    // -- printf("ROOT->NAME %s\n", root->name);

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    struct inode *temp = (struct inode *)malloc(sizeof(struct inode));

    struct inode *parent;
    if(strcmp(path, "/") == 0) parent = root;
    else parent = resolve_path(path, 2);

    if(parent != NULL && parent->st_nlink > 2) {
        for(int i = 0; i < parent->st_nlink - 2; i++){
            //struct inode *temp = root->children[0];
            // -- printf("PARENT->CHILD %d\n", parent->chls[i]);
            read_disk_inode((parent->chls[i] * sizeof(struct inode)) + INODE_OFFSET, temp);
            // -- printf("CHILD %i: %s\n",i, temp->name);
            filler(buf, temp->name, NULL, 0);
        }
    }

    return 0;
}


static int file_open(const char* path, struct fuse_file_info* fi)
{

    // -- printf("file_OPEN IS CALLED");
    // -- printf("Opened a file! %s\n",path);
    clrscr();

    char *a = strdup(path);

    // make a filehandle and enclose in fi, extract in file_write and do write
    filehandle *fh = (filehandle *)malloc(sizeof(filehandle));
    if(fh == NULL)
    {
        return -1;
    }
    // Locate the file
    char *hj = strdup(path);
    fh->node = resolve_path(path,0); // 0 because we dont want to create a dir, use this when multi level directories are ready
    //fh->node = createChild(root, hj+1 ,0); // 0 because we dont want to create a dir
    if(fh->node == NULL)
    {
        return -1;
    }
    fi->fh = fh;    
    return 0;

}

static int file_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    // -- printf("file CREATE IS CALLED");
    // -- printf("Created a file %s\n",path);
    clrscr();

    return file_open(path,fi);
}


static struct fuse_operations file_oper = {
        .getattr    = file_getattr,
        .readdir    = file_readdir,
        .open       = file_open,
        .mkdir      = file_mkdir,
        .create     = file_create,
};

int main(int argc, char *argv[])
{
    if(init_storage()==-1) // initialises storage and creates a root directory
    {
        return -1;
    }
    printf("\n\n\n\n\n\n\n\n");

    return fuse_main(argc, argv, &file_oper, NULL);
}