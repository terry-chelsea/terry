/*
 * =====================================================================================
 *
 *       Filename:  Traverse_filesystem.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/10/12 22:20:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include "Traverse.h"

char full_path[PATH_LENGTH];
DEAL_WITH_FILE file_func;

//整个路径名保存在dir_name执行的字符数组中.
int traverse_top_dir(char *dir_name , DEAL_WITH_FILE func)
{
    DIR *bucket_dentry = opendir(dir_name);
    if(NULL == bucket_dentry)
    {
        LOG_ERROR("In traverse_bucket : opendir directory %s error: %s" , dir_name , strerror(errno));
        return -1;
    }

    struct dirent *dirent_entry = NULL;
    while((dirent_entry = readdir(bucket_dentry)) != NULL)
    {
        char *sub_dir_name = dirent_entry->d_name;
        if(!strcmp(sub_dir_name , ".") || !strcmp(sub_dir_name , ".."))
            continue ;
        else if(dirent_entry->d_type != DT_DIR)                         //对于根目录下只遍历目录
            continue ;

        //添加上新的目录的名字，组成一个用户根目录的路径名
        int backup_length = append_sub_name(dir_name , sub_dir_name , PATH_LENGTH);
        if(backup_length < 0)
        {
            LOG_WARNING("In traverse_user_dir : path is too long : %s/%s" , dir_name , sub_dir_name);
            continue ;
        }

        if(traverse_second_dir(dir_name , func) < 0)
        {
            LOG_ERROR("In traverse_bucket : traverse use dir %s error!" , dir_name);
        }
        
        //还原原路径名
        clear_last_dir_name(dir_name , backup_length);
    }

    closedir(bucket_dentry);
    return 0;
}

int traverse_second_dir(char *user_dir_name , DEAL_WITH_FILE func)
{
    DIR *user_dentry = opendir(user_dir_name);
    if(NULL == user_dentry)
    {
        LOG_ERROR("In traverse_user_dir : opendir %s error : %s" , user_dir_name , strerror(errno));
        return -1;
    }

    struct dirent *user_entry = NULL;
    while((user_entry = readdir(user_dentry)) != NULL)
    {
        char *file_name = user_entry->d_name;
        if(!strcmp(file_name , ".") || !strcmp(file_name , ".."))
            continue ;

        if(DT_DIR != user_entry->d_type && DT_REG != user_entry->d_type)
        {
            LOG_INFO("In traverse_user_dir : read a special file : %s/%s" , user_dir_name , file_name);
            continue ;
        }

        int backup_length = append_sub_name(user_dir_name , file_name , PATH_LENGTH);
        if(backup_length < 0)
        {
            LOG_WARNING("In traverse_user_dir : path is too long : %s/%s" , user_dir_name , file_name);
            continue ;
        }
        if(DT_REG == user_entry->d_type)
        {
            func(user_dir_name);
        }
        if(DT_DIR == user_entry->d_type)
        {
            if(recursive_traverse_dir(user_dir_name , PATH_LENGTH , func) < 0)
                LOG_ERROR("In traverse_user_dir : recursive_traverse_dir error !");
        }

        clear_last_dir_name(user_dir_name , backup_length);
    }

    closedir(user_dentry);

    return 0;
}

int recursive_traverse_dir(char *dir_name , int max_path_length , DEAL_WITH_FILE func)
{
    DIR *dir_dentry = opendir(dir_name);
    if(NULL == dir_dentry)
    {
        LOG_ERROR("In recursive_traverse_dir : opendir %s error : %s" , dir_name , strerror(errno));
        return -1;
    }

    struct dirent *dir_entry = NULL;
    while((dir_entry = readdir(dir_dentry)) != NULL)
    {
        char *file_name = dir_entry->d_name;
        if(!strcmp(file_name , ".") || !strcmp(file_name , ".."))
            continue ;
       
        if(DT_DIR != dir_entry->d_type && DT_REG != dir_entry->d_type)
        {
            LOG_INFO("In recursive_traverse_dir : read a special file : %s/%s" , dir_name , file_name);
            continue ;
        }

        int backup_length = append_sub_name(dir_name , file_name , max_path_length);
        if(backup_length < 0)
        {
            LOG_WARNING("In traverse_user_dir : path is too long : %s/%s" , dir_name , file_name);
            continue ;
        }
        if(DT_REG == dir_entry->d_type)
        {
            func(dir_name);
        }
        if(DT_DIR == dir_entry->d_type)
        {
            recursive_traverse_dir(dir_name , max_path_length , func);
        }

        clear_last_dir_name(dir_name , backup_length);
    }
    closedir(dir_dentry);

    return 0;
}

//返回值为原路径名的长度，如果超过限制返回-1.
int append_sub_name(char *src_dir_name , char *sub_dir_name , int max_length)
{
    int dir_name_current_length = strlen(src_dir_name);           
    int backup_length = dir_name_current_length;                //保存原路径的长度

    int sub_dir_name_length = strlen(sub_dir_name);
    if(dir_name_current_length + sub_dir_name_length + 1 >= max_length)
        return -1;

    src_dir_name[dir_name_current_length ++] = '/';                     //添加分隔符
    memcpy(src_dir_name + dir_name_current_length , sub_dir_name , sub_dir_name_length);  //添加新的目录名
    dir_name_current_length += sub_dir_name_length;

    src_dir_name[dir_name_current_length] = '\0';

    return backup_length;
}

void clear_last_dir_name(char *src_dir_name , int last_slash)
{
    src_dir_name[last_slash] = '\0';
}

