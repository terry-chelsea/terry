#!/usr/bin/python

import os
import sys

user_directory = "./users/"

def calculate_blocks(file) : 
    file_blocks = 0
    try : 
        fp = open(file , "r")
    except IOError : 
        print "file " , file , " not exist ..."
        return -1

    line = 1
    block_line = 0
    for data in fp.readlines() : 
        if block_line > 0 : 
            line += 1
            block_line -= 1
            if(len(data.strip()) != 40) : 
                print "line " , line , " in  file " , file , " not illegal ... "
                return -1
            continue
        else : 
            block_line = int(data)
            file_blocks += block_line

    return file_blocks

if __name__ == "__main__" : 
    blocks = 0;
    if len(sys.argv) >= 2 : 
        user_directory = sys.argv[1]
    file_names = os.listdir(user_directory)
    for file in file_names : 
        lines = calculate_blocks(user_directory + file)
        if lines < 0 : 
            print "file " , file , " has something wrong..."
        else :
            blocks += lines

    print "Finish dealing with all files , sum %d blocks ..." %blocks

