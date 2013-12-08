#!/usr/bin/python

import sys
import socket
import struct
import time

fake_server_port = 12345
mu_data_root = "/home/terry/mu_root/"
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
        return 

    conn.close()
    return peer

def get_buckets(conn , type , cmd) : 
    buckets = []
    try : 
        buf = conn.recv(struct.calcsize("ii"))
        recv_cmd , num = struct.unpack("ii")
        if recv_cmd != cmd : 
            print type , " rule command error , current command : " , recv_cmd
            return 
        if num != 0 : 
            print "This is a " , type , " server node ..."
            bucket_size = struct.calcsize("i")
            buf = conn.recv(bucket_size * num)
            for i in range(num) : 
                one_bucket = buf[i * bucket_size : (i + 1) * bucket_size]
                nr = struct.unpack("i")
                buckets.append(nr)
    except Exception as e : 
        print "Socket error while recving " , type , " bucket : " , e
        return 

    return buckets

def send_finish_msg(conn , err) : 
    buf = struct.pack("ii" , finish_all_job_command , err)
    try : 
        conn.sendall(buf)
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


def get_enough_data(conn , length) : 
    cur_len = 0
    data = ""
    while cur_len < length : 
        try : 
            buf = conn.recv(length)
            data += buf
            cur_len += len(buf)
        except Exception as e :
            print "Get enough error : " , e
            return 

    return data


using_characters = "abcdefghijklmnopqrstuvwxyz_"
using_digits = "0123456789"

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


def get_file_name()

def generate_user_tree(root)

def deal_with_user_file(user , data) : 

def deal_with_infohash(infohash) : 


def do_server_work(conn) : 
    header_size = calcsize("ii")
    user_size = calcsize("i")
    while(True) : 
        try : 
            buf = conn.recv(header_size)
            cmd , length = struct.unpack("ii" , buf)
            if cmd == send_MU_command : 
                user = conn.recv(user_size)
                user_file = get_enough_data(conn , length)
                if not user_file : 
                    print "Get User " , user , " file data failed ..."
                    return -1

                if deal_with_user_file(user , user_file)  : 
                    print "Deal with user " , user , " error ..."
                    return -1

            elif cmd == send_SU_command : 
                infohash = get_enough_data(conn , 40)
                if not infohash : 
                    print "Get Infohash message failed ..."
                    return -1

                if deal_with_infohash(infohash) : 
                    print "Deal with infohash message failed ..."
                    return -1

            elif cmd == finish_send_all_job_command : 
                print "Successfully recieved all center message ..."
                break
            else : 
                print "Get undefined command while do server work , command : " , cmd
                return -1
        except Exception as e:
            print "socket error while recv center message ..."
            return -1

    return 0


if __name__ == "__main__" : 
    if(len(sys.argv) != 2) : 
        print "./server this_ip"
        sys.exit(-1)
        
    conn = start_server((sys.argv[1] , fake_server_port))
    if conn : 
        print "start fake server failed ..."
        sys.exit(-1)
    else : 
        print "Successfully create connection on " , conn.getsockname() , "<======>" , conn.getpeername()

    buckets = get_all_buckets(conn)
    if buckets : 
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
            if create_su_buckets(buckets[1]) : 
                print "Create SU buckets failed ..."
                sys.exit(-1)
            else : 
                print "Create SU buckets success ..."


    ret = do_server_work(conn)
    if ret : 
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
