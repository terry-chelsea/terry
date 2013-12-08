/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  I want this module start as a process...
 *
 *        Version:  1.0
 *        Created:  08/08/13 11:03:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Debug.h"

typedef struct ThreadInfo Tinfo;
typedef struct config Configuration;

int parse_config_file(const char *file_name);
void free_config_info();
void start_rubbish_server( , thread);
Tinfo *create_work_thread();
void  stop_work_thread();

int main(int argc , char *argv[])
{
    if(argc != 2)
    {
        printf("./CMD config_file \n");
        return -1;
    }

    START_DEBUG(argv[0] , OUT_SCR , DEBUG);
    Configuration *config = parse_config_file(argv[1]);
    if(NULL == config)
    {
        LOG_ERROR("Parse congiguration file %s failed ..." , argv[1]);
        return -1;
    }
     
    Tinfo *thread = create_work_thread();
    if(NULL == thread)
    {
        LOG_ERROR("Create work thread failed ...");
        return -1;
    }

    //in MU rubbish recycler process , only one connection communicate with CS
    //in this connection , recieve all heartbeat and request message ...
    //when in process , CS want connect the listen port , do accept and close it 
    //right now ...
    start_rubbish_server(all_config_info , thread);

    LOG_INFO_TIME("Rubbish recycler will exit ...");

    delete thread;
    thread = NULL;
    
    free_config_info(config);
    FINISH_DEBUG();
    return 0;
}
