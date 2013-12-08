#!/usr/bin/python

import sys
import random
import os
import time

users_directory = "./users/"
user_max_number = 0
block_size = 40
thread_number = 20
each_thread_tasks = 50
min_block_size = 0
max_block_size = 50
files_per_user = 1000

total_blocks = 0
total_finish_users = 0

start_time_second = int(time.time())


def new_file_size(min , max) : 
    return random.randint(min , max)

def get_user_number() : 
    global user_max_number 

    user_id = random.randint(0 , user_max_number)
    while os.path.exists(users_directory + str(user_id)) : 
        user_id = random.randint(0 , user_max_number)

    return user_id

all_characters = "0123456789abcde"
def fake_data(size) : 
    buffer = ""
    for i in range(size) : 
        buffer += random.choice(all_characters)

    return buffer


def create_fake_file(fp , blocks) : 

    fp.write(str(blocks) + "\n")
    for i in range(blocks) : 
        buf = fake_data(block_size)
        fp.write(buf + "\n")



def create_fake_data(users) : 
        global total_finish_users
        global total_blocks

        user_blocks = 0
        for i in range(users) : 
            id = get_user_number()
            fp = open(users_directory + str(id) , "w")
            for j in range(files_per_user) : 
                blocks = new_file_size(min_block_size , max_block_size)
                create_fake_file(fp , blocks)
                user_blocks += blocks
            fp.close()

            total_finish_users += 1
            if(total_finish_users % 10 == 0) : 
                print "finish " , total_finish_users , " users , cost " , int(time.time()) - start_time_second , " seconds ..."

        total_blocks += user_blocks



if __name__ == "__main__" : 
    if(len(sys.argv) != 5) : 
        print "./cmd user_number files_per_user block_min block_max"
        sys.exit(-1)

    users = int(sys.argv[1]);
    files = int(sys.argv[2]);
    min = int(sys.argv[3])
    max = int(sys.argv[4])
    
    user_max_number = users * 10000

    files_per_user = files
    max_block_size = max
    min_block_size = min
    ret = create_fake_data(users)
    if ret : 
        print "Create fake datas failed ..."
    else : 
        print "Create fake datas success total bocks : " , total_blocks  , " cost " , int(time.time()) - start_time_second , "seconds for all ..."
