#!/usr/bin/python

generate_list = "make normal_list.o"
generate_request = "make request.o"
listline = "./listline "
listline_source = " ./listline.c "
gcc_header = "gcc -o "
source = " normal_list.o request.o -g -O0 -Wall -Werror -lpthread -lm -DATOMIC "
debug_flag = " -DDEBUG"
thread_flag = " -DMTHREAD"
tmp_file = "./tmp"

import sys
import os
import string

def execute_cmd(cmd , out = None) :
#    output = " > /dev/null 2> /dev/null"
    output = ""
    exec_cmd = cmd
    if(out == None) : 
        exec_cmd = cmd + output;
    else :
        exec_cmd = cmd + " > " + out + " 2> " + out
        
    if(os.system(exec_cmd) != 0) :
            sys.exit()

def exec_test_process(name , number) :
    result = []

    for i in range(number) : 
        execute_cmd(name , tmp_file)
        fp = open(tmp_file)
        conts = fp.readlines()
        if(len(conts) != 1) :
            print "process %s execute failed !!!" % listline
            fp.close()
            execute_cmd("rm " + tmp_file)
            sys.exit(-1)
        else :
            tm = string.atof(conts[0])
            result.append(tm)

        fp.close()

    execute_cmd("rm " + tmp_file)
    return result

def test_debug(number) :
    compile_cmd = gcc_header + listline + listline_source + source + debug_flag
    execute_cmd(compile_cmd);
    main_thread = exec_test_process(listline , number)

    compile_cmd = gcc_header + listline + listline_source + source + debug_flag + thread_flag
    execute_cmd(compile_cmd);
    mutil_thread = exec_test_process(listline , number)

    compile_cmd = gcc_header + listline + listline_source + source
    execute_cmd(compile_cmd);
    list_ret = exec_test_process(listline , number)

    main_sum = 0.0
    thread_sum = 0.0
    list_sum = 0.0

    if(len(main_thread) != len(mutil_thread)) :
        print "Length is not equal !!!"
        sys.exit(-1)

    for i in main_thread :
        main_sum += i
    for i in mutil_thread : 
        thread_sum += i
    for i in list_ret : 
        list_sum += i

    print "Use origin test %d times : " %number
    print main_thread
    print "One thread test result : %lf seconds in average..." % (main_sum/number)
    print mutil_thread
    print "Mutil thread test result : %lf seconds in average..." % (thread_sum/number)
    print list_ret
    print "Listline thread test result : %lf seconds in average..." % (list_sum/number)


if __name__ == "__main__" :
    if(len(sys.argv) != 2) : 
        print "Usage : ./test.py  test_times..."
        sys.exit(-1)
    
    numbers = string.atoi(sys.argv[1])
    if(numbers < 0 or numbers > 100) :
        numbers = 16

    execute_cmd(generate_list)
    execute_cmd(generate_request)
    test_debug(numbers)
