#!/usr/bin/python

import sys
import random
import hashlib
import os
import threading
import time

users_directory = "./users/"
user_max_number = 0
block_size = 20000
thread_number = 20
each_thread_tasks = 50
hash = hashlib.sha1()
min_block_size = 0
max_block_size = 50
files_per_user = 1000

total_blocks = 0
total_finish_users = 0

mutex = threading.Lock()
start_time_second = int(time.time())


def new_file_size(min , max) : 
    return random.randint(min , max)

def get_user_number() : 
    global user_max_number 

    user_id = random.randint(0 , user_max_number)
    while os.path.exists(users_directory + str(user_id)) : 
        user_id = random.randint(0 , user_max_number)

    return user_id

all_characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890`!@#$%^&*()~-+=_|,.<>:;{}[]"
def fake_data(size) : 
    buffer = ""
    for i in range(size) : 
        buffer += random.choice(all_characters)

    return buffer


def create_fake_file(fp , blocks) : 

    fp.write(str(blocks) + "\n")
    for i in range(blocks) : 
        buf = fake_data(block_size)
        hash.update(buf)
        fp.write(hash.hexdigest() + "\n")


class MyThread(threading.Thread):    
    def run(self):
        global total_finish_users
        global total_blocks

        user_blocks = 0
        for i in range(each_thread_tasks) : 
            id = get_user_number()
            fp = open(users_directory + str(id) , "w")
            for j in range(files) : 
                blocks = new_file_size(min_block_size , max_block_size)
                user_blocks += blocks
                create_fake_file(fp , blocks )
            fp.close()
            mutex.acquire()
            total_finish_users += 1
            if(total_finish_users % 10 == 0) : 
                print "finish " , total_finish_users , " users , cost " , int(time.time()) - start_time_second , " seconds ..."
            mutex.release()

        mutex.acquire()
        total_blocks += user_blocks
        mutex.release()


def create_fake_data() : 
    threads = []
    for i in range(thread_number) : 
        t = MyThread()
        threads.append(t)

    for t in threads : 
        t.start()

    for t in threads : 
        t.join()


if __name__ == "__main__" : 
    if(len(sys.argv) != 6) : 
        print "./cmd user_number files_per_user block_min block_max(MB) thread_number"
        sys.exit(-1)

    users = int(sys.argv[1]);
    files = int(sys.argv[2]);
    min = int(sys.argv[3])
    max = int(sys.argv[4])
    
    user_max_number = users * 10000
    thread_number = int(sys.argv[5])
    if thread_number > 32 : 
        thread_number = 32

    each_thread_tasks = users / thread_number
    print "real users number : " , each_thread_tasks * thread_number

    files_per_user = files
    max_block_size = max
    min_block_size = min
    ret = create_fake_data()
    if ret : 
        print "Create fake datas failed ..."
    else : 
        print "Create fake datas success total bocks : " , total_blocks  , " cost " , int(time.time()) - start_time_second , "seconds for all ..."
