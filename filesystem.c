/*******************************************************************************
 *  Author:   Alexander Bezobchuk                                              *
 *  Please do not reproduce                                                    *
 *                                                                             *
 *  This project represents a fictional type of UNIX filesytem. Many of the    *
 *  various commands that can be used on UNIX, this project will implement.    *
 *  This Filesystem is composed of various structures of linked lists that all *
 *  depend on one another. Such functions that operate on a Filesystem include *
 *  navigation, creation of files and directories, printing contents and       *
 *  current locations. The structures of linked lists are optimized for        *
 *  navigation and quick access to files and directories. Note: This project   *
 *  does not free any dynamically allocated memory. In addition to project 3,  *
 *  a few more functions have been included in order to insure that, at the    *
 *  users request, there will be no memory leaks and all dynamically allocated *
 *  memory will be freed.                                                      *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filesystem.h"

/* Given the specified directory, the function will enter the names of all the 
 * files and sub directories in the directory into an array of strings and 
 * sort them. The functionw will also print the names after sorting. */
static void sort_and_print(Directory *);

/* Checks to see if the string is a name of a sub directory in the current 
 * directory. */
static int is_dir(Directory *, char *);

/* Removes the contents within the given directory, including the directory
 * itself. In other words, the function removes anything beyond and including 
 * sub directories of the given directory and the directory itself. */
static void remove_contents(Directory *);

/* Every call to mkfs() must initialize the parameter Filesystem in such a way 
 * that each returned value represents a different filesystem, so calling mkfs 
 * several times will not cause separate Filesystem variables to share any files 
 * or directories. The result of calling any of the other functions on a 
 * Filesystem variable before mkfs() is called on it is undefined. The result 
 * of calling any of the other functions is also undefined if mkfs() was first 
 * called, but its argument was just NULL. The function initialzes and mallocs
 * any neccessary components of the root directory. After this function is 
 * called, the current directory will be the root directory. 
 */
void mkfs(Filesystem *files)
{
    if (files != NULL)
    {
        files->root = malloc(sizeof(Directory));
        if (files->root != NULL)
        {
            files->root->dir_name = malloc(2);
            if (files->root->dir_name != NULL)
            {
                strcpy(files->root->dir_name, "/");
                files->root->file_list = NULL;
                files->root->sub_dir_list = NULL;
                files->root->parent_dir = files->root;
                files->curr_dir = files->root;
            }
            else
            {
                printf("Memory allocation failed!\n");
                exit(1);
            }
        }
        else
        {
            printf("Memory allocation failed!\n");
            exit(1);
        }
    }
}

/* This function’s usual effect is to create a file, if it does not already 
 * exist in the Filesystem files. If files is NULL, the function exits 
 * immediately, since NULL takes priority over all other errors. Otherwise,
 * the function will return 0 or another error code.
 */
int touch(Filesystem *files, const char arg[])
{
    
    if (files != NULL && arg != NULL)
    {
        
        File *curr_file = files->curr_dir->file_list, *new_file;
        Sub_directory *curr_s_dir;
        
        /* If arg is an empty string. */
        if (*arg == '\0')
            return -1;
        
        /* If arg is the name of a sub-directory or a file that already exists 
         * in the current directory, or is . (a single period), .., or / . */
        else if ((strcmp(arg, ".") == 0) || (strcmp(arg, "..") == 0)
                 || (strcmp(arg, "/") == 0))
            return 0;
        
        /* Check for existing files with the same name. */
        while (curr_file != NULL)
        {
            if (strcmp(arg, curr_file->file_name) == 0)
                return 0;
            curr_file = curr_file->next;
        }
        
        /* Check for existing sub directories with the same name. */
        if (files->curr_dir->sub_dir_list != NULL)
        {
            curr_s_dir = files->curr_dir->sub_dir_list;
            
            while (curr_s_dir != NULL)
            {
                if (strcmp(arg, curr_s_dir->curr_sub->dir_name) == 0)
                    return 0;
                curr_s_dir = curr_s_dir->next;
            }
        }
        
        /* By now, there are no files/directories with the same name. */
        curr_file = files->curr_dir->file_list;
        new_file = malloc(sizeof(File));
        new_file->file_name = malloc(strlen(arg) + 1);
        
        if (new_file != NULL && new_file->file_name != NULL)
        {
            strcpy(new_file->file_name, arg);
            new_file->next = NULL;
            
            if (curr_file == NULL)
            {
                files->curr_dir->file_list = new_file;
                return 0;
            }
            else
            {
                while (curr_file->next != NULL)
                    curr_file = curr_file->next;
                
                curr_file->next = new_file;
                return 0;
            }
        }
        else
        {
            printf("Memory allocation failed!\n");
            exit(1);
        }
    }
    return 0;
}

/* The usual effect of this function is to create a sub-directory in the 
 * current directory. If files is NULL, the function exits 
 * immediately, since NULL takes priority over all other errors. Otherwise,
 * the function will return 0 or another error code.
 */
int mkdir(Filesystem *files, const char arg[])
{
    
    if (files != NULL && arg != NULL)
    {
        File *curr_file = files->curr_dir->file_list;
        Directory *new_dir;
        Sub_directory *curr_s_dir, *new_s_dir;
        
        /* If arg is an empty string. */
        if (*arg == '\0')
            return -1;
        
        /* If arg is the name of a file or of a sub-directory that already 
         * exists in the current directory, or is . (a single period), or .., 
         * or / */
        else if ((strcmp(arg, ".") == 0) || (strcmp(arg, "..") == 0)
                 || (strcmp(arg, "/") == 0))
            return -2;
        
        /* Check for existing files with the same name. */
        while (curr_file != NULL)
        {
            if (strcmp(arg, curr_file->file_name) == 0)
                return -2;
            curr_file = curr_file->next;
        }
        
        /* Check for existing sub directories with the same name. */
        if (files->curr_dir->sub_dir_list != NULL)
        {
            curr_s_dir = files->curr_dir->sub_dir_list;
            
            while (curr_s_dir != NULL)
            {
                if (strcmp(arg, curr_s_dir->curr_sub->dir_name) == 0)
                    return -2;
                curr_s_dir = curr_s_dir->next;
            }
        }
        
        /* At this point, there should not be any files or sub directories in 
         * the current directory with the same name in the parameter. 
         * The function will proceed to make the sub directory. */
        new_dir = malloc(sizeof(Directory));
        new_dir->dir_name = malloc(strlen(arg) + 1);
        new_s_dir = malloc(sizeof(Sub_directory));
        
        if (new_dir != NULL && new_dir->dir_name != NULL && new_s_dir != NULL)
        {
            strcpy(new_dir->dir_name, arg);
            new_dir->file_list = NULL;
            new_dir->sub_dir_list = NULL;
            new_dir->parent_dir = files->curr_dir;
            new_s_dir->curr_sub = new_dir;
            new_s_dir->next = NULL;
            
            if (files->curr_dir->sub_dir_list == NULL)
            {
                files->curr_dir->sub_dir_list = new_s_dir;
                return 0;
            }
            
            else
            {
                curr_s_dir = files->curr_dir->sub_dir_list;
                
                while (curr_s_dir->next != NULL)
                    curr_s_dir = curr_s_dir->next;
                
                curr_s_dir->next = new_s_dir;
                return 0;
            }
        }
        else
        {
            printf("Memory allocation failed!\n");
            exit(1);
        }
    }
    return 0;
}

/* This function’s usual effect is to change the current directory. If files is 
 * NULL, the function exits immediately, since NULL takes priority over 
 * all other errors. Otherwise, the function will return 0 or another error
 * code.
 */
int cd(Filesystem *files, const char arg[])
{
    if (files != NULL && arg != NULL)
    {
        File *curr_file = files->curr_dir->file_list;
        Sub_directory *curr_s_dir;
        int exist_file_or_dir = 0;
        
        /* If arg is /, the root directory becomes the new current directory. */
        if (strcmp(arg, "/") == 0)
        {
            files->curr_dir = files->root;
            return 0;
        }
        
        /* If arg is . (a single period), or is an empty string, the function 
         * has no effect. */
        if ((strcmp(arg, ".") == 0) || *arg == '\0')
            return 0;
        
        /* If arg is .., the parent directory of the current directory becomes 
         * the new current directory. */
        if (strcmp(arg, "..") == 0)
        {
            if (strcmp(files->curr_dir->dir_name, "/") == 0)
                return 0;
            else
            {
                files->curr_dir = files->curr_dir->parent_dir;
                return 0;
            }
        }
        
        /* If arg is the name of a file that exists in the current directory. */
        while (curr_file != NULL)
        {
            if (strcmp(arg, curr_file->file_name) == 0)
                return -2;
            
            curr_file = curr_file->next;
        }
        
        /* If arg is a name that does not refer to an existing file or 
         * directory in the current directory. */
        if (files->curr_dir->sub_dir_list != NULL)
        {
            curr_s_dir = files->curr_dir->sub_dir_list;
            
            while (curr_s_dir != NULL)
            {
                if (strcmp(arg, curr_s_dir->curr_sub->dir_name) == 0)
                    exist_file_or_dir = 1;
                
                curr_s_dir = curr_s_dir->next;
            }
        }
        
        if (!exist_file_or_dir)
            return -1;
        
        /* At this point arg is the name of a directory that exists as an 
         * immediate sub-directory of the current directory. */
        
        /* Find and point to the directory, and have the Filesystem's current
         * directory now point to that. */
        curr_s_dir = files->curr_dir->sub_dir_list;
        while (curr_s_dir->next != NULL && 
               (strcmp(arg, curr_s_dir->curr_sub->dir_name) != 0))
            curr_s_dir = curr_s_dir->next;
        
        files->curr_dir = curr_s_dir->curr_sub;
        return 0;
    }
    else
        return 0;
}

/* This function’s usual effect is to list the files and sub-directories of the
 * current directory, or the files and sub-directories of its argument if that
 * is a sub-directory, or to list its argument if that is a file.
 */
int ls(Filesystem files, const char arg[])
{
    if (arg != NULL)
    {
        
        int exist_file = 0, exist_dir = 0;
        File *curr_file = files.curr_dir->file_list;
        Sub_directory *curr_s_dir = files.curr_dir->sub_dir_list;
        
        
        /* If arg is . (a single period) or the empty string, the function prints
         * all the files and sub directories of the current directory. (If root,
         * then there may be no sub directories or files). */
        if (strcmp(arg, ".") == 0 || *arg == '\0')
        {
            sort_and_print(files.curr_dir);
            return 0;
        }
        
        /* If arg is / (the root), the function prints all the files and sub 
         * directories of the root directory . */
        if (strcmp(arg, "/") == 0)
        {
            sort_and_print(files.root);
            return 0;
        }
        
        /* If arg is .. (double peroid), the function prints all the files and
         * sub directories of the parent directory. (If the current directory is the
         * root, then function acts the same as "." or "/").*/
        if (strcmp(arg, "..") == 0)
        {
            if (strcmp("/", files.curr_dir->dir_name) == 0)
            {
                sort_and_print(files.root);
                return 0; 
            }
            else
            {
                sort_and_print(files.curr_dir->parent_dir);
                return 0;
            }
        }
        
        /* Check for existing files with the same name. */
        while (curr_file != NULL)
        {
            if (strcmp(arg, curr_file->file_name) == 0)
            {
                exist_file = 1;
                break;
            }
            
            curr_file = curr_file->next;
        }
        
        /* Check for existing sub directories with the same name. */
        while (curr_s_dir != NULL)
        {
            if (strcmp(arg, curr_s_dir->curr_sub->dir_name) == 0)
            {
                exist_dir = 1;
                break;
            }
            
            curr_s_dir = curr_s_dir->next;
        }
        
        /* If arg is the name of a file that exists in the current directory. */
        if (exist_file)
        {
            printf("%s\n", curr_file->file_name);
            return 0;
        }
        
        
        /* If arg is a name that does not refer to an existing file or directory 
         * in the current directory. */
        if (!exist_dir && !exist_file)
            return -1;
        
        /* At this point, arg must be the name of an exisiting sub directory, the 
         * function will print all the files and sub directories of the specified 
         * sub directory of the current directory. */
        sort_and_print(curr_s_dir->curr_sub);
        return 0;
        
    }
    else
        return 0;
}

/* This function’s effect is to print the full path from the root to the current 
 * directory. The path begins with a forward–slash character (/), signifying 
 * that the root is the base of the entire filesystem and the parent or ancestor 
 * of all other files and directories. Following the slash, the names of all of 
 * the directories between the root directory and the current directory are 
 * printed in order from the top of the filesystem (the immediate sub-directory
 * of the root), ending with the name of the current directory. The pwd function 
 * always produces at least some output. If the root directory is the current 
 * directory only a single forward–slash is printed. After mkfs() is called, the
 * curr directory will always be the root.
 */
void pwd(Filesystem files)
{
    char **path = NULL; 
    int directories = 0, i;
    Directory *dir = files.curr_dir;
    
    /* If the current directory is the root. */
    if (strcmp(files.curr_dir->dir_name, "/") == 0)
    {
        printf("/\n");
        return;
    }
    
    /* Count the number of directories, exlcuding the root. */
    while (strcmp("/", dir->dir_name) != 0) 
    {
        directories++;
        dir = dir->parent_dir;
    }
    path = malloc(sizeof(char *) * directories);
    
    if (path == NULL)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    
    dir = files.curr_dir; /* Reset the tmp dir pointer */
    i = 0;
    
    /* The array of strings will first allocate memory, add "/" and then the 
     * name of the current directory, going back parent by parent. (Note: 
     * this array will contain the names in reverse order!!!!) Also, it will
     * not include the root, since it adds "/" to every directory name. */
    while (i < directories)
    {
        path[i] = malloc(strlen(dir->dir_name) + 1); 
        
        if (path[i] == NULL)
        {
            printf("Memory allocation failed!\n");
            exit(1);
        }
        strcpy(path[i++], dir->dir_name);
        dir = dir->parent_dir;
    }
    
    /* Print the names in the array of strings in REVERSE order to get correct
     * output!!!!!! */
    i = directories - 1;
    while (i >= 0)
    {
        printf("/");
        printf("%s", path[i--]);
    }
    printf("\n");
    
    /* Free all malloc'd memory */
    i = 0;
    while (i < directories)
    {
        free(path[i++]);
    }
    free(path);
}

static void sort_and_print(Directory *dir)
{
    if (dir != NULL)
    {
        char **s_arr, *tmp_s;
        File *curr_file = dir->file_list;
        Sub_directory *curr_s_dir = dir->sub_dir_list;
        int elements = 0, i, j;
        
        
        /* Find number of total files and sub dir. */
        while (curr_file != NULL)
        {
            elements++;
            curr_file = curr_file->next;
        }
        while (curr_s_dir != NULL)
        {
            elements++;
            curr_s_dir = curr_s_dir->next;
        }
        
        s_arr = malloc(sizeof(char *) * elements);
        if (s_arr == NULL)
        {
            printf("Memory allocation failed!\n");
            exit(1);
        }
        i = 0;
        
        /* Reset pointers. */
        curr_file = dir->file_list;
        curr_s_dir = dir->sub_dir_list;
        
        /* Enter all files into array of strings. */
        while (curr_file != NULL)
        {
            s_arr[i] = malloc(strlen(curr_file->file_name) + 1);
            if (s_arr[i] == NULL)
            {
                printf("Memory allocation failed!\n");
                exit(1);
            }
            strcpy(s_arr[i++], curr_file->file_name);
            curr_file = curr_file->next;
        }
        
        /* Enter all sub directories into array of strings. */
        while (curr_s_dir != NULL)
        {
            s_arr[i] = malloc(strlen(curr_s_dir->curr_sub->dir_name) + 1);
            if (s_arr[i] == NULL)
            {
                printf("Memory allocation failed!\n");
                exit(1);
            }
            strcpy(s_arr[i++], curr_s_dir->curr_sub->dir_name);
            curr_s_dir = curr_s_dir->next;
        }
        
        /* By now, s_arr will be an array of strings of both any existing files
         * and any existing sub directories, however it will be unsorted!! */
        
        /* Sort the strings in the array using bubble sort.
         * (Slow, I know hahahaha) but its easy to impliment. (= */
        j = elements - 1;
        while (j > 0)
        {
            for (i = 1; i <= j; i++)
            {
                if (strcmp(s_arr[i-1], s_arr[i]) > 0)
                {
                    tmp_s = s_arr[i-1];
                    s_arr[i-1] = s_arr[i];
                    s_arr[i] = tmp_s;
                }
            }      
            j--;
        }
        
        /* Print the sorted names, and append "/" to sub directories. */
        i = 0;
        while (i < elements)
        {
            if (is_dir(dir, s_arr[i])) /* To find out if the name is a dir.   */
                printf("%s/\n", s_arr[i++]);
            
            else
                printf("%s\n", s_arr[i++]);
        }
        
        /* Free all malloc'd memory */
        i = 0;
        while (i < elements)
        {
            free(s_arr[i++]);
        }
        free(s_arr);
    }
}

static int is_dir(Directory *dir, char *str)
{
    Sub_directory *curr_s_dir = dir->sub_dir_list;
    
    /* Simply checks if the current directory contains any sub directories with
     * the given name str. */
    while (curr_s_dir != NULL)
    {
        if (strcmp(str, curr_s_dir->curr_sub->dir_name) == 0)
            return 1;
        
        curr_s_dir = curr_s_dir->next;
    }
    return 0;
}


/* This function will deallocate any dynamically-allocated memory that is used 
 * by the Filesystem variable that its parameter files points to, destroying the 
 * filesystem and all its data in the process. The parameter files will use no 
 * dynamically-allocated memory at all after this function is called. (i.e. the
 * filesystem variable will not contain any memory leaks. */
void rmfs(Filesystem *files)
{
    if (files != NULL)
    {
        remove_contents(files->root);
    }
}

/* This function’s usual effect is to remove a file or a directory from the 
 * current directory. In removing files and directories this function will 
 * ensure that no memory leaks occur. The last file or directory could be 
 * removed from a directory, causing it to become an empty directory with no 
 * contents, but the current directory can never be removed.*/
int rm(Filesystem *files, const char arg[])
{
    if (files != NULL && arg != NULL)
    {
        File *prev_file = NULL, *curr_file = files->curr_dir->file_list, 
        *tmp_file;
        Sub_directory *prev_s_d = NULL, 
        *curr_s_d = files->curr_dir->sub_dir_list, *tmp_s_d;
        int exist_file = 0, exist_dir = 0;
        
        /* If arg is an empty string */
        if (*arg == '\0')
            return -3;
        
        /* If arg is . (a single period), .., or / */
        if ((strcmp(arg, ".") == 0) || (strcmp(arg, "..") == 0) || 
            (strcmp(arg, "/") == 0))
            return -2;
        
        while (curr_file != NULL)
        {
            if(strcmp(arg, curr_file->file_name) == 0)
            {
                exist_file = 1;
                break;
            }
            prev_file = curr_file;
            curr_file = curr_file->next;
        }
        
        while (curr_s_d != NULL)
        {
            if (strcmp(arg, curr_s_d->curr_sub->dir_name) == 0)
            {
                exist_dir = 1;
                break;
            }
            prev_s_d = curr_s_d;
            curr_s_d = curr_s_d->next;
        }
        
        /* If the current directory does not contain a file or sub directory 
         * with the name that arg refers to. */
        if (!exist_dir && !exist_file)
            return -1;
        
        /* At this point there must exist a file OR sub directory within the 
         * current directory with the name that arg refers to. */
        
        /* If there exists a file with the name that arg refers to, remove it */
        if (exist_file)
        {
            tmp_file = curr_file;
            
            if (prev_file == NULL) /* If the file to remove is the first file */
            {
                files->curr_dir->file_list = curr_file->next;
                free(tmp_file->file_name);
                free(tmp_file);
                return 0;
            }
            
            prev_file->next = curr_file->next;
            free(tmp_file->file_name);
            free(tmp_file);
            return 0;
        }
        
        else
        {
            tmp_s_d = curr_s_d;
            
            if (prev_s_d == NULL) /* If the sub dir to remove is the first */
            {
                files->curr_dir->sub_dir_list = curr_s_d->next;
                remove_contents(tmp_s_d->curr_sub);
                free(tmp_s_d);
                return 0;
            }
            
            prev_s_d->next = curr_s_d->next;
            remove_contents(tmp_s_d->curr_sub);
            free(tmp_s_d);
            return 0;
        }
    }
    else
        return 0;
}

static void remove_contents(Directory *dir)
{
    Sub_directory *travel_sub_dir, *tmp_sub_dir, **prev_sub_dir;
    Directory *tmp_dir;
    File *tmp_file, *tmp_file_next;
    travel_sub_dir = dir->sub_dir_list;
    prev_sub_dir = &dir->sub_dir_list;
    
    while (dir->sub_dir_list != NULL)
    {
        
        while (travel_sub_dir->next != NULL)
        {
            prev_sub_dir = &travel_sub_dir->next;
            travel_sub_dir = travel_sub_dir->next;
        }
        
        if (travel_sub_dir->curr_sub->sub_dir_list != NULL)
        {
            prev_sub_dir = &travel_sub_dir->curr_sub->sub_dir_list;
            travel_sub_dir = travel_sub_dir->curr_sub->sub_dir_list;
        }
        
        else
        {
            tmp_dir = travel_sub_dir->curr_sub;
            tmp_sub_dir = travel_sub_dir;
            travel_sub_dir = dir->sub_dir_list;
            
            if (tmp_dir->file_list != NULL)
            {
                tmp_file = tmp_dir->file_list;
                while (tmp_file != NULL)
                {
                    tmp_file_next = tmp_file->next;
                    free(tmp_file->file_name);
                    free(tmp_file);
                    tmp_file = tmp_file_next;
                }
            }
            tmp_dir->file_list = NULL;
            free(tmp_dir->dir_name);
            free(tmp_dir);
            free(tmp_sub_dir);
            *prev_sub_dir = NULL;
            prev_sub_dir = &dir->sub_dir_list;
        }
    }
    
    if (dir->file_list != NULL)
    {
        tmp_file = dir->file_list;
        while (tmp_file != NULL)
        {
            tmp_file_next = tmp_file->next;
            free(tmp_file->file_name);
            free(tmp_file);
            tmp_file = tmp_file_next;
        }
    }
    free(dir->dir_name);
    free(dir);
}

/* This function’s usual effect is to change the name of a file or directory. 
 * The name of the current directory cannot be changed by this function, nor can 
 * the name of any directory between the root and the current directory. */
int re_name(Filesystem *files, const char arg1[], const char arg2[])
{
    if (files != NULL && arg1 != NULL && arg2 != NULL)
    {
        int exist_dir = 0, exist_file = 0, exist_arg2 = 0;
        File *curr_file = files->curr_dir->file_list;
        Sub_directory *curr_s_d = files->curr_dir->sub_dir_list;
        
        /* If arg1 or arg2 is an empty string */
        if (*arg1 == '\0' || *arg2 == '\0')
            return -2;
        
        /* If arg1 or arg2 is either ".", "..", or "/" */
        if ((strcmp(arg1, ".") == 0) || (strcmp(arg1, "..") == 0) || 
            (strcmp(arg1, "/") == 0) || (strcmp(arg2, ".") == 0) ||
            (strcmp(arg2, "..") == 0) || (strcmp(arg2, "/") == 0))
            return -3;
        
        while (curr_file != NULL)
        {
            if (strcmp(arg1, curr_file->file_name) == 0)
            {
                exist_file = 1;
                break;
            }
            if (strcmp(arg2, curr_file->file_name) == 0)
                exist_arg2 = 1;
            
            curr_file = curr_file->next;
        }
        
        while (curr_s_d != NULL)
        {
            if (strcmp(arg1, curr_s_d->curr_sub->dir_name) == 0)
            {
                exist_dir = 1;
                break;
            }
            if (strcmp(arg2, curr_s_d->curr_sub->dir_name) == 0)
                exist_arg2 = 1;
            
            curr_s_d = curr_s_d->next;
        }
        
        /* If arg2 is a different name from arg1 but there is already a file or
         * directory in the current directory with the name arg2 */
        if (exist_arg2 && (strcmp(arg1, arg2) != 0))
            return -3;
        
        /* If there does not exist a file or sub directory in the current 
         * directory with the name of arg1 */
        if (!exist_dir && !exist_file)
            return -1;
        
        /* If arg1 is the name of a file or directory that exists in the current 
         * directory at that time, and arg2 is the same as arg1 */
        if ((exist_file || exist_dir) && (strcmp(arg1, arg2) == 0))
            return -4;
        
        /* If arg1 is the name of a file or directory that exists in the current 
         * directory at that time, and there is not already a file or directory 
         * in the current directory named arg2, the function will try to change 
         * arg1’s name to arg2 */
        if (exist_file)
        {
            free(curr_file->file_name);
            curr_file->file_name = malloc(strlen(arg2) + 1);
            
            if (curr_file->file_name == NULL)
            {
                printf("Memory allocation failed!\n");
                exit(1);
            }
            
            strcpy(curr_file->file_name, arg2);
            return 0;
        }
        else
        {
            free(curr_s_d->curr_sub->dir_name);
            curr_s_d->curr_sub->dir_name = malloc(strlen(arg2) + 1);
            
            if (curr_s_d->curr_sub->dir_name == NULL)
            {
                printf("Memory allocation failed!\n");
                exit(1);
            }
            
            strcpy(curr_s_d->curr_sub->dir_name, arg2);
            return 0;
        }
    }
    else
        return 0;
}

/*******************************************************************************
 *                               END OF PROGRAM                                *
 ******************************************************************************/