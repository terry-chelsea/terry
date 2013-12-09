#!/usr/bin/python
#coding=utf-8

#该文件用来使用生成的user目录下的文件产生实际的MU和SU数据
#对于MU需要的数据时一个目录树，下面有多个桶构成的子目录树，每个桶又是由多个用户构成的
#使用网络的方式创建MU数据
#对于SU需要的是数据库信息，使用远程MYSQL的方式在本地进行操作...
#对于每个节点，使用一个线程与其保持连接...

import xml.etree.ElementTree
import sys
import os
import socket
import struct
import thread
import threading
import pdb
import MySQLdb
import random
import time

mu = "MU"
su = "SU"
#user_directory = "/home/terry/Rubbish_recycler/fake_data/users/"
user_directory = "./users/"
server_port = 12345
start_time = 0

def parse_MU_or_SU(doc , root) : 
    MOD = "/MOD"
    IP = "/IP"
    BACKUP = "/BACKUP"

    node = doc.findall(root + MOD)
    if len(node) != 1 : 
        print type , " mod error ..."
        return 
    mu_mod = int(node[0].text)

    node = doc.findall(root + BACKUP)
    if len(node) != 1 : 
        print type , " node error ..."
        return 
    mu_backup = int(node[0].text)

    IPs = []
    nodes = doc.findall(root + IP)

    for node in nodes : 
        IPs.append(node.text)

    return (mu_mod , mu_backup , IPs)

def parse_config_file(file) : 
    try : 
        fp = open(file , "r")
    except IOError : 
        print "Open configure file " , file , " failed ..."
        return 

    prefix = "//"

    config = {}

    doc = xml.etree.ElementTree.parse(file)
#parse MU info...
    conf = parse_MU_or_SU(doc , prefix + mu)
    if not conf : 
        return 
    config[mu] = conf

    conf = parse_MU_or_SU(doc , prefix + su)
    if not conf : 
        return 
    config[su] = conf

    return config

def generate_rule(bucket , IPs , backup) : 
    ip_num = len(IPs)
    counter = 0

    rule = []
    for i in range(bucket) : 
        bucket_rule = []
        for j in range(backup) : 
            bucket_rule.append(IPs[counter % ip_num])
            counter += 1
        rule.append(bucket_rule)

    return rule

def display_rule(rule , type) : 
    print type , " rule : "
    bucket = 0
    for i in rule : 
        print "Bucket " , bucket , " : " ,
        for j in i : 
            print j , "  " , 
        bucket += 1
        print ""


def assign_rule(config) : 
    mu_bucket = pow(2 , config[mu][0])
    su_bucket = pow(2 , config[su][0])

    mu_rule = generate_rule(mu_bucket , config[mu][2] , config[mu][1])
    su_rule = generate_rule(su_bucket , config[su][2] , config[su][1])
    
    if 1 : 
        display_rule(mu_rule , mu)
        display_rule(su_rule , su)

    return (mu_rule , su_rule)

def close_all_sockets(sockets) : 
    for sock in sockets : 
        sockets[sock].close()

def close_all_connections(conns) : 
    for ip in conns : 
        conns[ip][0].close()
        conns[ip][1].close()

#["SU" : (mod , node , [IPS...])]

def connect_all_nodes(config) : 
    all_node_ip = []
    all_node_ip.extend(config[2])
#    all_node_ip.extend(config[su][2])
    # clear repeat IP...
    all_node_ip = {}.fromkeys(all_node_ip).keys()
    all_sockets = {}
    for ip in all_node_ip : 
        socket_ = socket.socket(socket.AF_INET , socket.SOCK_STREAM)
        err = socket_.connect_ex((ip , server_port))
        if err : 
            print "Connect to " , ip , " and port " , server_port , " failed ..."
            close_all_sockets(all_sockets)
            return 
        all_sockets[ip] = socket_

    return all_sockets


send_MU_rule_command = 0
send_SU_rule_command = 1
send_MU_command = 2
send_SU_command = 3
finish_send_all_job_command = 4
finish_all_job_command = 5

def send_all_buckets(cmd , rule , ip , socket_) : 
    bucket_nr = 0
    this_ip_buckets = []
    for i in rule : 
        if ip in i : 
            this_ip_buckets.append(bucket_nr)
        bucket_nr += 1
    print this_ip_buckets

#if this is a server , have some rules...
    try : 
        #header include command and data length ...
        header = struct.pack("ii" , cmd , len(this_ip_buckets) * struct.calcsize("i"))
        socket_.sendall(header)
        bucket_str = ""
        for i in this_ip_buckets : 
            bucket_str += struct.pack("i" , i)
        socket_.sendall(bucket_str)
    except socket.error : 
        print "Send Bucket infomations to " , ip , " failed ..."
        return -1

    return 0

        
#only send MU rules , SU data generated here...
def send_rule_to_node(sockets , rules) : 
    for ip in sockets : 
        ret = send_all_buckets(send_MU_rule_command , rules[0] , ip , sockets[ip])
        if ret : 
            print "Send MU rule to " , ip , " failed ..."
            return -1
#        ret = send_all_buckets(send_SU_command , rules[1] , ip , sockets[ip])
#        if ret : 
#            print "Send SU rule to " , ip , " failed ..."
#            return -1

def get_mu_bucket_nr(user_id , all_buckets) : 
    return user_id % all_buckets

hash_map = {'0' : 48 , '1' : 49 , '2' : 50 , '3' : 51 , '4' : 52 , \
        '5' : 53 , '6' : 54 , '7' : 55 , '8' : 56 , '9' : 57 ,\
                'a' : 97 , 'b' : 98 , 'c' : 99 , 'd' : 100 , 'e' : 101 , 'f' : 102}
def get_su_bucket_nr(infohash , all_buckets) : 
    value = 1315423911
    for ch in infohash : 
        value = (value << 5) + hash_map[ch] + (value >> 2)

    return value % all_buckets

# only send MU data , this is all user file data ...
def send_all_MU_date_to_server(users , sockets , rules) : 
    mu_buckets_num = len(rules)

    for user in users : 
        try :
            fp = open(user_directory + user)
        except IOError : 
            print "Open file " , user_directory + user , " failed ..."
            return -1
        
        file_data = fp.read()
        # send all file content to MU and 
        user_id = int(user)
        header = struct.pack("iii" , send_MU_command , len(file_data) , user_id)
        nr = get_mu_bucket_nr(user_id , mu_buckets_num)
        info = header + file_data
        try : 
            for ip in {}.fromkeys(rules[nr]).keys() : 
                sockets[ip].sendall(info)
        except socket.error : 
            print "Send file data of user " , user , " to MU " , ip , " failed ..."
            fp.close()
            return -1
        fp.close()

        print "Send all file data of user " , user_id , " to node : " ,{}.fromkeys(rules[nr]).keys()

    try : 
        buf = struct.pack("ii" , finish_send_all_job_command , 0)
        for ip in sockets : 
            sockets[ip].sendall(buf)
            print "Send finish all command success to node " , ip

    except Exception as e : 
        print "Send finish all command faied : " , e
        return 

    return 0

su_database_port = 3306
su_database_user = "root"
su_database_pwd = "mysql"
su_database_db = "su_test"
su_bucket_table = "Bucket_Table"
bucket_table_attr = "(Bucket_ID int not null primary key , Sequence bigint , Old_Sequence bigint , State int , Size bigint)engine=MyISAM DEFAULT CHARSET=utf8"
su_mod_table = "Mod_Table"
mod_table_attr = "(Mod_Num int primary key not null)DEFAULT CHARSET=utf8"
su_bucket_prefix = "Block_Table_"
infohash_table_attr = "(Key_Idx char(2) not null , Infohash varchar(64) primary key not null , Sequence bigint , Size int , Del int , \
        Time bigint , Path varchar(255) , KEY `key_index` (`Key_Idx`))ENGINE=MyISAM , DEFAULT CHARSET=utf8"
create_table_prefix = "create table "
insert_table_prefix = "insert into "
update_table_prefix = "update "
normal_size = 524288
default_path = "/home/terry/su_root"

def deal_with_infohash(conn , infohash , bucket_nr) : 
#    pdb.set_trace() 

    size = normal_size
    if random.randint(1 , 1000) == 0 : 
        size = random.randint(1024 , normal_size)
    try : 
        insert_sql = insert_table_prefix + su_bucket_prefix + str(bucket_nr) + " values(%s , %s , %s , %s , %s , %s , %s)"
        conn.execute(insert_sql , (infohash[0 : 2] , infohash , 0 , size , 0 , int(time.time()) , default_path))
#        update_sql = update_table_prefix + su_bucket_table + " set Size = Size + %s where Bucket_ID = %s"
#        conn.execute(update_sql , (size , bucket_nr))
    except Exception as e : 
        print "Execute insert and update an infohash , size " , size , " error : " , e 
        return -1

    return 0



def connect_all_mysql(config) : 
    su_ips = config[2]
    su_ips = {}.fromkeys(su_ips).keys()
    all_conns = {}
    for ip in su_ips : 
        try : 
            conn = MySQLdb.connect(host = ip , port = 3306 , user = su_database_user , \
                    passwd = su_database_pwd , db = su_database_db , charset = "utf8")
        except Exception as e : 
            close_all_connections(all_conns)
            print "Create mysql connection error : " , e
            return ()
        all_conns[ip] = (conn , conn.cursor())

    return all_conns


def create_su_buckets(conns , rules) : 
    bucket_table_sql = create_table_prefix + su_bucket_table  + bucket_table_attr
    mod_table_sql = create_table_prefix + su_mod_table + mod_table_attr
    
    buckets = len(rules)
    for ip in conns :
        this_ip_buckets = []
        for i in range(buckets) : 
            for rule_ip in rules[i] : 
                if(rule_ip == ip) : 
                    this_ip_buckets.append(i)
        cur = conns[ip][1]
        try :
            cur.execute(bucket_table_sql)
            cur.execute(mod_table_sql)

            for bucket in  this_ip_buckets : 
                insert_sql = insert_table_prefix + su_bucket_table + " values(%s , %s , %s , %s , %s)"
                cur.execute(insert_sql , (bucket , 0 , 0 , 0 , 0))
            insert_sql = insert_table_prefix + su_mod_table + " values(%s)"
            cur.execute(insert_sql , buckets)

        except Exception as e : 
            print "Execute create bucket table and mod table failed : " , e
            return -1
        
    buckets = len(rules)
    for nr in range(buckets) : 
        infohash_table_sql = create_table_prefix + su_bucket_prefix + str(nr) + infohash_table_attr
        for ip in rules[nr] : 
            try : 
                conns[ip][1].execute(infohash_table_sql)
            except Exception as e : 
                close_all_connections(conns)
                print "Execute create infohash table failed : " , e
                return -1
    return 0

def send_all_SU_data_to_server(users , conns , rules) : 
    global start_time

    su_buckets_num = len(rules)
    su_counter = 0

    for user in users : 
        try :
            fp = open(user_directory + user)
        except Exception as e :
            print "Open file " , user_directory + user , " failed : " , e
            return -1

        line = 1
        block_line = 0
        for data in fp.readlines() : 
            if block_line > 0 : 
                block_line -= 1
                data = data.strip()
                if(len(data) != 40) : 
                    print "line " , line , " in  file " , file , " not illegal ... "
                    fp.close()
                    return -1

                nr = get_su_bucket_nr(data , su_buckets_num)
                try : 
                    for ip in {}.fromkeys(rules[nr]).keys() : 
                        if deal_with_infohash(conns[ip][1] , data , nr) : 
                            print "Deal with infohash " , data , " to node " , ip , " failed ..."
                            fp.close()
                            return -1

                    su_counter += 1
                    if not su_counter % 10000 : 
                        cur_time = int(time.time())
                        print "finish insert " , su_counter , " infohashs , use %d seconds ..." %(cur_time - start_time)
                        start_time = cur_time
                except  Exception as e : 
                    print "Send file data of user " , user , " to MU " , ip , " failed : " , e
                    fp.close()
                    return -1
            else : 
                block_line = int(data.strip())

            line += 1
        
        fp.close()

    return su_counter

socket_timeout = 5
def wait_to_the_end(sockets) : 
    while sockets : 
        alive_ips = 0
        for ip in sockets : 
            if(sockets[ip]) : 
                alive_ips += 1
                try : 
                    sockets[ip].settimeout(socket_timeout)
                    buf = sockets[ip].recv(struct.calcsize("ii"))
                    if(len(buf) == 0) : 
                        print "!!!!!!!!!!!!!get close infomation from " , ip
                    if(len(buf) != struct.calcsize("ii")) : 
                        print "recv data from node " , ip , " failed ..."
                    else : 
                        cmd , err = struct.unpack("ii" , buf)
                        if(cmd != finish_all_job_command) : 
                            print "recv undefined command " , cmd , " from node " , ip
                        else : 
                            if err != 0 : 
                                print "Server in " , ip , "do job faied ..."
                            else : 
                                print "Server in " , ip , " finish all jobs ..."
                    sockets[ip].close()
                    sockets[ip] = None
                        
                except socket.timeout : 
                    print "In last " , socket_timeout , " seconds , node " , ip , " not finish ..."
                    continue ;

        if alive_ips == 0 : 
            break;

if __name__ == "__main__" : 
    if(len(sys.argv) != 2) : 
        print "./center config_file"
        sys.exit(-1)

    config = parse_config_file(sys.argv[1])
    if config :
        print "parse config file success ..."
        print config
    else :
        print "Parse config file failed ..."
        sys.exit(-1)

    all_rule = assign_rule(config)

#    all_sockets = connect_all_nodes(config[mu])
    all_sockets = True
    if not all_sockets : 
        print "Connect to server failed ..."
        sys.exit(-1)
    else : 
        print "Connect to all server successfully..."

#    ret = send_rule_to_node(all_sockets , all_rule)
    ret = 0
    if ret : 
        print "Send all rule to nodes failed ..."
        close_all_sockets(all_sockets)
        sys.exit(-1)
    else : 
        print "Send all rule to server successfully..."

    all_users = os.listdir(user_directory)
    print "Get all users infomations successfully , There are " , len(all_users) , " users ..."

#    ret = send_all_MU_date_to_server(all_users , all_sockets , all_rule[0])
    ret = 0
    if ret : 
        print "Send all user(MU) infomations to server failed ..."
        close_all_sockets(all_sockets)
        sys.exit(-1)
    else : 
        print "Send all user(MU) infomations to server success ..."

#    wait_to_the_end(all_sockets)

    all_conns = connect_all_mysql(config[su])
    if not all_conns : 
        print "Connect to mysqls failed ..."
        sys.exit(-1)
    else : 
        print "Create connection to all mysqls success ..."

    if create_su_buckets(all_conns , all_rule[1]) : 
        print "Create all SU buckets failed ..."
        sys.exit(-1)
    else : 
        print "Create all SU buckets success ..."

    start_time = int(time.time())
    cnt = send_all_SU_data_to_server(all_users , all_conns , all_rule[1])
    if cnt < 0 :
        print "send all Infohash infomations to servers failed ..."
        sys.exit(-1)
    else : 
        print "Send all infohash to SU success , counter " , cnt

    close_all_connections(all_conns)

    print "all data deploy finish ..."
