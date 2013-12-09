#!/usr/bin/python

import sys
import socket
import struct
import time
import random
import pdb
import os

fake_server_port = 12345
mu_data_root = "/home/terry/mu_root/"
su_data_root = "/home/terry/su_root/"
su_database_port = 3306
su_database_user = "user"
su_database_pwd = "mysql"
su_database_db = "c_t_store"
su_bucket_table = "Bucket_Table"
su_mod_table = "Mod_Table"
su_bucket_prefix = "Block_Table_"
last_time = int(time.time())

send_MU_rule_command = 0
send_SU_rule_command = 1
send_MU_command = 2
send_SU_command = 3
finish_send_all_job_command = 4
finish_all_job_command = 5

def start_server(addr) : 
    try : 
        conn = socket.socket(socket.AF_INET , socket.SOCK_STREAM)
        conn.bind(addr)
        conn.listen(5)
        peer , address = conn.accept()
        print "Get connection from " , address[0] , " port " , address[1]
    except Exception as e: 
        print "socket failed : " , e
        conn.close()
        return 

    return peer

def get_buckets(conn , type , cmd) : 
    buckets = []
    try : 
        buf = conn.recv(struct.calcsize("ii"))
        recv_cmd , num = struct.unpack("ii" , buf)
        if recv_cmd != cmd : 
            print type , " rule command error , current command : " , recv_cmd
            return 
        if num != 0 : 
            print "This is a " , type , " server node ..."
            bucket_size = struct.calcsize("i")
            buf = conn.recv(num)
            for i in range(num / bucket_size) : 
                one_bucket = buf[i * bucket_size : (i + 1) * bucket_size]
                nr = struct.unpack("i" , one_bucket)
                buckets.append(nr[0])
    except Exception as e : 
        print "Socket error while recving " , type , " bucket : " , e
        return 

    return buckets

def send_finish_msg(conn , err) : 
    buf = struct.pack("ii" , finish_all_job_command , err)
    try : 
        conn.sendall(buf)
        buf = conn.recv(1)
        if len(buf) == 0 : 
            print "The opposite has Closed this connection ..."
    except Exception as e : 
        print "Send finish data faied : " , e

def get_all_buckets(conn) : 
    su_buckets = []
    print "Wait for MU bucket infomations ..."
    mu_buckets = get_buckets(conn , "MU" , send_MU_rule_command)
    if mu_buckets == None : 
        return 
    su_buckets = get_buckets(conn , "SU" , send_SU_rule_command)
    if su_buckets == None : 
        return 
    return (mu_buckets , su_buckets)


def get_enough_data(conn , length) : 
    cur_len = 0
    data = ""
    while cur_len < length : 
        try : 
            buf = conn.recv(length - cur_len)
            data += buf
            get_len = len(buf)
            cur_len += get_len
            
        except Exception as e :
            print "Get enough error : " , e
            return 

    return data



using_characters = "abcdefghijklmnopqrstuvwxyz_"
using_digits = "0123456789"
file_name_max_length = 32
file_name_min_length = 4
max_file_per_directory = 32
min_file_per_directory = 8
directory_max_deep = 3

dir_key = "dir"
deep_key = "deep"
cur_files = "cur_files"
max_files = "max_files"


def new_file_name() : 
    name = random.choice(using_characters)
    name_len = random.randint(file_name_min_length , file_name_max_length)

    for i in range(name_len) : 
        name += random.choice(using_characters + using_digits)

    return name

#first character is a number , this is limit the max subdirectory in this directory...
def new_directory_name() : 
    name = str(int(random.choice(using_digits)) + 1)
    name_len = random.randint(file_name_min_length , file_name_max_length)

    for i in range(name_len) : 
        name += random.choice(using_characters + using_digits)

    return name

def make_new_directory(dir) : 
    try : 
        os.mkdir(dir)
    except Exception as e : 
        print "Create directory " , dir , " failed : " , e
        return -1
    return 0

def create_mu_buckets(buckets) :
    for buck in buckets : 
        dir_name = mu_data_root + str(buck)
        if make_new_directory(dir_name) : 
            print "Create new root directory for bucket " , buck , " failed ..."
            return -1

    return 0

def create_su_buckets(buckets) : 
    fps = {}
    for buck in buckets : 
        file_name = su_data_root + str(buck)
        try : 
            fp = open(file_name , "w")
        except Exception as e : 
            print "Create SU bucket file " , file_name , " failed : " , e
            return 
        fps[buck] = fp

    return fps

def delete_all_fps(all_fps) : 
    for buck in all_fps : 
        all_fps[buck].close()

def get_subdirectorys(cur) : 
    dir_nums = 0
    items = os.listdir(cur)
    for item in  items : 
        if os.path.isdir(cur + '/' + item) : 
            dir_nums += 1

    return dir_nums


def get_mu_bucket_nr(user_id , all_buckets) : 
    return user_id % all_buckets

def deal_with_one_file(infohashs , cur_dir) : 
#    pdb.set_trace()

    # in root directory ...
    if cur_dir[deep_key] == 0 : 
        if cur_dir[cur_files] == cur_dir[max_files] : 
            sub_dir_name = new_directory_name()
            sub_dir_fullname = cur_dir[dir_key] + '/' + sub_dir_name
            if make_new_directory(sub_dir_fullname) : 
                print "create directory failed : " , sub_dir_fullname
                return -1
            cur_dir[dir_key] = sub_dir_fullname
            cur_dir[cur_files] = 0
            cur_dir[max_files] = random.randint(min_file_per_directory , max_file_per_directory)
            cur_dir[deep_key] += 1
    
    if cur_dir[cur_files] == cur_dir[max_files] : 
        cur_deep = cur_dir[deep_key]
        cur_dir_name = cur_dir[dir_key]
        try : 
            cur_max_subdir = int(cur_dir_name[cur_dir_name.rfind('/') + 1])
        except Exception as e : 
            print "Here in line 185 error : " , cur_dir_name , " reason : " , e
        cur_real_subdir = get_subdirectorys(cur_dir_name)

        if cur_deep == directory_max_deep or cur_real_subdir == cur_max_subdir:
            while(cur_deep > 0) : 
                cur_dir_name = cur_dir_name[ : cur_dir_name.rfind('/')]
                cur_deep -= 1
                cur_real_subdir = get_subdirectorys(cur_dir_name)
                try : 
                    cur_max_subdir = int(cur_dir_name[cur_dir_name.rfind('/') + 1])
                except Exception as e : 
                    print "Here in line 196 error : " , cur_dir_name , " reason : " , e
                if cur_real_subdir < cur_max_subdir : 
                    break
        sub_dir_name = new_directory_name()
        sub_dir_fullname = cur_dir_name + '/' + sub_dir_name
        if make_new_directory(sub_dir_fullname) : 
            print "create directory failed : " , sub_dir_fullname
            return -1
        cur_dir[dir_key] = sub_dir_fullname
        cur_dir[cur_files] = 0
        cur_dir[max_files] = random.randint(min_file_per_directory , max_file_per_directory)
        cur_dir[deep_key] = cur_deep + 1

    try : 
        file_name = cur_dir[dir_key] + '/' + new_file_name()
        fp = open(file_name , "w")
    except Exception as e : 
        print "Create new file " , file_name , " failed : " , e
        return -1

    for hash in infohashs : 
        fp.write(hash + '\n')
    cur_dir[cur_files] += 1

    fp.close()

    return 0

def deal_with_user_file(user , data , root) : 
    print "Start create user directory for user " , user
    max_file_number = random.randint(min_file_per_directory , max_file_per_directory)
    dir_info = {dir_key : root , deep_key : 0 , cur_files : 0 , max_files : max_file_number}

    file_datas = data.split("\n")
    one_file = []
    block_line = 0
    for line in file_datas[ : -1] : 
        if block_line > 0 : 
            block_line -= 1
            if(len(line) != 40) : 
                print "This infohash length is not 40 : " , line
                return -1
            one_file.append(line)
        else : 
            try : 
                block_line = int(line)
            except Exception as e : 
                print "change line to int error : " , e , " line is " , line , " end..."
            if one_file and deal_with_one_file(one_file , dir_info) : 
                print "deal with one new file failed , current directory : " , cur_dir
                return -1
            del one_file[:]


def deal_with_infohash(fp , infohash) : 
    fp.write(infohash + '\n')
    return 0


def do_server_work(conn , buckets_num , all_fps) : 
    header_size = struct.calcsize("ii")
    user_size = struct.calcsize("i")
    all_users = 0
    all_infohash = 0
    all_fp_keys = all_fps.keys()
    while(True) : 
        try : 
            buf = get_enough_data(conn , header_size)
            cmd , length = struct.unpack("ii" , buf)
            if cmd == send_MU_command : 
                user = conn.recv(user_size)
                user_id = struct.unpack("i" , user)[0]
                user_file = get_enough_data(conn , length)
                if not user_file : 
                    print "Get User " , user , " file data failed ..."
                    return ()

                user_root = mu_data_root + str(get_mu_bucket_nr(user_id , buckets_num)) + "/" + str(user_id)
                if make_new_directory(user_root) : 
                    print "Create directory " , user_root , " for user " , user_id , " failed ..."
                    return ()

                if deal_with_user_file(user_id , user_file , user_root)  : 
                    print "Deal with user " , user , " error ..."
                    return ()
                all_users += 1

            elif cmd == send_SU_command : 
                bucket = conn.recv(user_size)
                bucket_nr = struct.unpack("i" , bucket)[0]
                infohash = get_enough_data(conn , 40)
                if not infohash : 
                    print "Get Infohash message failed ..."
                    return ()

                if bucket_nr not in all_fp_keys: 
                    print "Wrong bucket number recieved : " , bucket_nr , " infohash : " , infohash
                    return ()

                if deal_with_infohash(all_fps[bucket_nr] , infohash) : 
                    print "Deal with infohash message failed ..."
                    return ()

                all_infohash += 1
            elif cmd == finish_send_all_job_command : 
                print "Successfully recieved all center message ..."
#                pdb.set_trace()
#                conn.close()
                return (all_users , all_infohash)
                
            else : 
                print "Get undefined command while do server work , command : " , cmd
                return ()
        except Exception as e:
            print "socket error while recv center message : " , e
            return ()



if __name__ == "__main__" : 
    if(len(sys.argv) != 2) : 
        print "./server this_ip"
        sys.exit(-1)
        
    conn = start_server((sys.argv[1] , fake_server_port))
    if not conn : 
        print "start fake server failed ..."
        sys.exit(-1)
    else : 
        print "Successfully create connection on " , conn.getsockname() , "<======>" , conn.getpeername()


    buckets = get_all_buckets(conn)
    if not buckets : 
        print "Get all buckets failed ..." 
        conn.close()
        sys.exit(-1)
    else : 
        if buckets[0] : 
            print "MU buckets : " , buckets[0]
            if create_mu_buckets(buckets[0]) : 
                print "Create MU buckets failed ..."
                sys.exit(-1)
            else : 
                print "Create MU buckets success ..."

        if buckets[1] :
            print "SU buckets : " , buckets[1]
            all_fps = create_su_buckets(buckets[1])
            if not all_fps : 
                print "Create SU buckets failed ..."
                sys.exit(-1)
            else : 
                print "Create SU buckets success ..."

    ret = do_server_work(conn , len(buckets[0]) , all_fps)
    if not ret : 
        print "Do server work failed ..."
        send_finish_msg(conn , 1)
        conn.close()
    else : 
        print "Do all server work success ..."
        if ret[0] : 
            print "This is a MU node , There are " , ret[0] , " users ..."
        if ret[1] : 
            print "This is a SU node , There are " , ret[1] , " infohashs ..."

    send_finish_msg(conn , 0)
    print "This server job finish ..."
