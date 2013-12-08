#ifndef _H_SIMPLE_LOG_H_
#define _H_SIMPLE_LOG_H_

#define START_DEBUG(type , level , file)    start_debug(type , level , file)
#define FINISH_DEBUG()       end_debug()

typedef enum
{
    INFO_MSG = 0 , 
    WARNING_MSG , 
    ERROR_MSG , 
    SYSCALL_MSG , 
    SYSCALL_EXIT_MSG , 
    EXIT_MSG , 
    UNKNOW_MSG
}MSG_TYPE;

#ifdef __cplusplus
extern "C"
{
#endif
    extern void start_debug(int type , int level , const char *filename);
    extern void end_debug();
    extern void print_info(MSG_TYPE type_nr , int line , const char *file , const char *format , ...);
    extern void print_binary_code(const void *structBuffer , int nSize);

#ifdef __cplusplus
}
#endif

typedef enum
{
    OUT_SCR = 1 ,
    OUT_FILE = 2 , 
    OUT_BOTH = 3 ,
}LOG_TYPE;

#define DEBUG

#ifndef DEBUG

#define LOG_INFO(format , ...)  
#define LOG_WARNING(format , ...)
#define LOG_BIN_CODE(ptr , len) 

#else

#define LOG_INFO(format , ...)  \
    print_info(INFO_MSG , __LINE__ , __FILE__ , format , ##__VA_ARGS__)

#define LOG_WARNING(format , ...) \
    print_info(WARNING_MSG , __LINE__ , __FILE__ , format ,## __VA_ARGS__)

#define LOG_BIN_CODE(ptr , len) \
    print_binary_code(ptr , len)

#endif
   
#define LOG_ERROR(format , ...) \
    print_info(ERROR_MSG , __LINE__ , __FILE__ , format , ##__VA_ARGS__)

#define LOG_SYSCALL(format , ...) \
    print_info(SYSCALL_MSG , __LINE__ , __FILE__ , format , ##__VA_ARGS__)

#define LOG_SYSCALL_EXIT(format , ...) \
    print_info(SYSCALL_EXIT_MSG , __LINE__ , __FILE__ , format , ##__VA_ARGS__)

#define LOG_EXIT(format , ...) \
    print_info(EXIT_MSG , __LINE__ , __FILE__ , format , ##__VA_ARGS__)


#endif
