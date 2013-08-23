#ifndef _file_system_internals_h
#define _file_system_internals_h

struct sub_dir;

/* A linked list of files. */
typedef struct file
{
    char *file_name;
    struct file *next;
}File;

/* A directory which contains a name, list of files, a pointer to a parent, and
 * a linked list of sub directories. */
typedef struct dir
{
    
    char *dir_name;
    File *file_list;
    struct sub_dir *sub_dir_list;
    struct dir *parent_dir;
    
}Directory;

/* A linked list of sub directories. */
typedef struct sub_dir
{
    Directory *curr_sub;
    struct sub_dir *next;
}Sub_directory;


/* The actualy filesystem contains a pointer to a root and a curr directory 
 * pointer to know what locationt the filesystem is at all times. */
typedef struct
{
    Directory *root;
    struct dir *curr_dir;    
    
}Filesystem;

#endif
