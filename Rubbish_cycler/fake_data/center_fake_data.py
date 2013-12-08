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

mu = "MU"
su = "SU"
user_directory = "/home/terry/Rubbish_recycler/fake_data/users/"
server_port = 12345

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

def connect_all_nodes(config) : 
    all_node_ip = []
    all_node_ip.extend(config[mu][2])
    all_node_ip.extend(config[su][2])
    # clear repeat IP...
    all_node_ip = {}.fromkeys(all_node_ip).keys()
    all_sockets = {}
    for ip in all_node_ip : 
        socket_ = socket.socket(socket.AF_INET , socket.SOCK_STREAM)
        err = socket_.connect_ex((ip , server_port))
        if err : 
            print "Connect to " , ip , " and port " , server_port , " failed ..."
            for sock in all_sockets : 
                all_sockets[sock].close()
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

def get_su_bucket_nr(infohash , all_buckets) : 
    value = 1315423911
    for ch in infohash : 
        value = (value << 5) + ch + (value >> 2)

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

def deal_with_infohash(infohash) : 
    return 
#    print infohash

def send_all_SU_data_to_server(users , sockets , rules) : 
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
                else : 
                    if deal_with_infohash(data) : 
                        print "Deal with infohash " , data , " failed ..."
                        fp.close()
                        return -1
                    su_counter += 1
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


#["SU" : (mod , node , [IPS...])]
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

    all_sockets = connect_all_nodes(config)
    if not all_sockets : 
        print "Connect to server failed ..."
        sys.exit(-1)
    else : 
        print "Connect to all server successfully..."

    ret = send_rule_to_node(all_sockets , all_rule)
    if ret : 
        print "Send all rule to nodes failed ..."
        for sock in all_sockets : 
            all_sockets[sock].close()
        sys.exit(-1)
    else : 
        print "Send all rule to server successfully..."

#    pdb.set_trace() 

    all_users = os.listdir(user_directory)
    print "Get all users infomations successfully , There are " , len(all_users) , " users ..."

    ret = send_all_MU_date_to_server(all_users , all_sockets , all_rule[0])
    if ret : 
        print "Send all user(MU) infomations to server failed ..."
        for sock in all_sockets : 
            all_sockets[sock].close()
        sys.exit(-1)
    else : 
        print "Send all user(MU) infomations to server success ..."

    wait_to_the_end(all_sockets)

#    cnt = send_all_SU_data_to_server(all_users , all_sockets , all_rule[1])
    cnt = 1
    if cnt < 0 :
        print "send all Infohash infomations to servers failed ..."
        sys.exit(-1)
    else : 
        print "Send all infohash to SU success , counter " , cnt

    print "all data deploy finish ..."
