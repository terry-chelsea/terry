#!/usr/bin/python

import sys
import os

deploy_root_dir = "/opt/cstore/deploy/"
temp_file_name = "outtmp"

tool_dir = "tools/"
module_dir = "modules/"
lib_dir = "libs/"
config_dir = "config/"

mysql_dir = "/usr/local/mysql/"
mysql_version = "5.1.48"
mysql_installed = False
odbc_deployed = False

def execute_cmd(cmd , flag = 1 , out = temp_file_name) :
    print "start " , cmd , " ..."
    if(os.system(cmd + " >" + out +  " 2>" + out) != 0) :
        print "<" , str(cmd) , ">" , " Execute failed !"
        if(flag) :
            sys.exit()
        else :
            return -1

    print "< " , cmd , " > success"
    return 0

def cd_dir(dir) :
    try :
	execute_cmd("rm " + temp_file_name , 0)
        os.chdir(dir)
    except OSError :
        print "directory < " , dir , " > is not exits..."
        sys.exit(-1)

    print "Current directory " , os.getcwd()

def decompress(file) : 
    execute_cmd("mkdir -p " + deploy_root_dir)
    execute_cmd("cp " + file + " " + deploy_root_dir)
    cd_dir(deploy_root_dir)

    execute_cmd("tar xvf " + file)
    execute_cmd("rm " + file)	

    return 0

#current directory is deploy_root_dir
def check_mysql() :
    ret = execute_cmd(mysql_dir + "/bin/mysql_config --version" , 0)
    if(ret == 0) :
        print "Mysql has been installed !"
        tmp = open(temp_file_name)
        version = tmp.readline().strip()
        if mysql_version != version :
            print "Mysql version is different ! Current version : " , version
            tmp.close()
            return -1

        tmp.close()
        return 1

    else :
        print "Mysql is not exist..."
        return 0

import time
def check_mysql_running() :
    sleep_tm = 0
    out_flag = 0
    while(out_flag) :
	if(sleep_tm > 30) :
	    print "Can not start mysql ... has been waiting for 30 seconds..."
	    return -1

	execute_cmd("ps -e | grep mysqld_safe")
	lines = 0
	fp = open(temp_file_name)
	for line in fp :
	    lines += 1
	    if(lines == 0) :
	        print "Mysql not running...wait for 1 second..."
	        time.sleep(1)
	        sleep_tm += 1
	        continue
	    else :
	        out_flag = 1
	        break
    return 0

def install_new_mysql() : 
    global mysql_installed

    cd_dir(deploy_root_dir + tool_dir)
    execute_cmd("groupadd mysql" , 0)
    execute_cmd("useradd mysql" , 0)
    execute_cmd("tar zxvf mysql-5.1.48.tar.gz")
    cd_dir(deploy_root_dir + tool_dir + "mysql-5.1.48")
    execute_cmd("mkdir -p build")
    cd_dir("./build")
    execute_cmd("../configure --prefix=" + mysql_dir)
    execute_cmd("make")
    execute_cmd("make install")
    execute_cmd("cp support-files/my-medium.cnf /etc/my.cnf")
    cd_dir(mysql_dir)
    execute_cmd("./bin/mysql_install_db --user=mysql")
    execute_cmd("chown -R root .")
    execute_cmd("chown -R mysql var")
    execute_cmd("./bin/mysqld_safe --user=mysql &")

    count = 0
    while(execute_cmd("./bin/mysqladmin -u root password 'mysql'" , 0) != 0) :
	count += 1
	print "can not change password , sleep for one second..."
	time.sleep(1)
	if(count > 30) :
	    print "Can not startup mysqld daemon ..."
	    sys.exit(-1)

    execute_cmd("ln -s /usr/local/mysql/bin/mysql /usr/bin/mysql" , 0)
    #test mysql running...
    execute_cmd("echo '' >" + temp_file_name)
    execute_cmd("mysql -uroot -pmysql < ./" + temp_file_name)
    mysql_installed = True

    

def wait_to_continue(info) :
    info += "continue or not(yes or no)..."
    while True :
        rin = raw_input(info)
        low = rin.strip().lower();
        if(low == "yes") :
            return 0
        elif (low == "no") :
            return -1
        else :
            info = "must input yes or no ..."

def install_mysql() :
    global mysql_installed

    ret = check_mysql()
    if(0 == ret) : 
        install_new_mysql()
    elif (-1 == ret) : 
        print "System has install it , uninstall it first..."
        sys.exit(0)
    else :
        cnt = wait_to_continue("MySQL has been installed...")
        if(0 == cnt) :
            return ;
        else :
            sys.exit(-1)

odbcfile = "odbc.ini"
odbcinst = "odbcinst.ini"
odbcpath = "/usr/local/unixODBC/"
odbconnecter = "libmyodbc3-3.51.29.so"
odbcso = "libodbc.so.1.0.0"
odbcinstso = "libodbcinst.so.1.0.0"
ldconfigpath = "/etc/ld.so.conf"

def get_sock_path() :
    tmppath = "/tmp/mysql.sock"
    varpath = "/var/lib/mysql/mysql.sock"
    varflag = True
    tmpflag = True
    if(os.system("ls " + tmppath) != 0) :
        tmpflag = False
    if(os.system("ls " + varpath) != 0) :
        varflag = False

    if(not tmpflag and not varflag) :
        print "MUst exist a mysql.sock in " , tmppath , " or " , varpath
        sys.exit(-1)
    
    if(tmpflag and varflag) :
        return 

    if(tmpflag) :
	execute_cmd("mkdir -p " + "/var/lib/mysql/")
        execute_cmd("ln -s " + tmppath + " " + varpath , 0)
    else :
	execute_cmd("mkdir -p " + "/var/lib/mysql/")
        execute_cmd("ln -s " + varpath + " " + tmppath , 0)

def deploy_odbc() :
    global odbc_deployed

    libpath = odbcpath + "lib/"
    configpath = odbcpath + "etc/"
    execute_cmd("mkdir -p " + libpath)
    execute_cmd("mkdir -p " + configpath)
    cd_dir(deploy_root_dir + config_dir)
    execute_cmd("cp " + odbcfile + " " + odbcinst + " " + configpath)
    cd_dir(deploy_root_dir + lib_dir)
    execute_cmd("cp " + odbconnecter + " " + odbcinstso + " " + odbcso + " " + libpath)
    cd_dir(libpath)
    execute_cmd("ln -s " + odbconnecter + " " + "./libmyodbc3.so" , 0)
    if(os.system("echo '" + libpath + "'" + " >> " + ldconfigpath) != 0) :
	print "echo " , libpath , " append to " , ldconfigpath , " error ..."
	sys.exit()

    execute_cmd("ldconfig")
    cd_dir(deploy_root_dir + tool_dir)

#assert mysql.sock is exist...
    execute_cmd("touch __tmpfile")
    execute_cmd("./isql test < ./__tmpfile")
    execute_cmd("rm __tmpfile")

    odbc_deployed = True


cstore_deploy_path = "/opt/nsdl/cstore/deploy/"
cstore_lib = "lib/"
need_libs = {"libmysqlclient" : ["libmysqlclient.so.16" , False] , 
            "libzdbc" : ["libzdbc.so" , False] , 
            "libzip" : ["libzip.so.1.0.0" , False] ,
            "libzdb" : ["libzdb.so.9.0.1" , False] , 
            "libprotobuf" : ["libprotobuf.so.7.0.0" , False]
            }
modules_name = {"cs" : "CS_RS" , "mu" : "MU" , "su" : "SU" , 
        "gw" : "Gateway" , "ca" : ("CA_LOGIN" , "CA_PORTAL")}


module_databases = {"su" : "c_t_store" , "cs" : "cs" , "ca" : "ca"}
config_files = {"mu" : "MUConfig.xml" , "su" : "SUConfig.xml" ,
        "cs" : "CSConfig.xml" , "gw" : ["Gateway.xml" , "config.xml"]}

modules = ("mu" , "su" , "cs" , "ca" , "gw" , "all")
notfound = "not found"
separator = "=>"
in_separator = "."

def get_dependance_libs(name) :
    one_libs = []

    execute_cmd("ldd " + name)
    fp = open(temp_file_name)
    for line in fp :
        #this lib is not found
        if(line.find(notfound) != -1) :
            lib_name = line.split(separator)[0]
            lib_name = lib_name.split(in_separator)[0]
            lib_name = lib_name.strip()
            if lib_name not in need_libs.keys() :
                print "%s need %d not found ..."  % (module , lib_name)
                print line
                sys.exit(-1)
            else :
                if (need_libs[lib_name][1]) :
                    continue 
                else :
                    one_libs.append(lib_name)
                    need_libs[lib_name][1] = True

    fp.close()
    return one_libs

def create_database(dbname) :
    cd_dir(deploy_root_dir)
    create_cmd = "create database " + dbname
    execute_cmd("echo '" + create_cmd + "' > " + temp_file_name)
    execute_cmd("mysql -uroot -pmysql < ./" + temp_file_name)
    execute_cmd("rm " + temp_file_name)


#cs need mysql...
def deploy_one_module(module) :
    global mysql_installed
    global odbc_deployed

    if(module not in modules[0 : 6]) :
        print "Unkonw module : %s" % module
        sys.exit(-1)

    if(module in modules[0 : 4]) :
        if(not mysql_installed) :
            install_mysql()

    if(module in modules[0 : 2]) :
        if(not odbc_deployed) :
            deploy_odbc()

    cd_dir(deploy_root_dir + module_dir)
    one_libs = []
    if(module == modules[3]) :
        one_libs.extend(get_dependance_libs(modules_name[module][0]))
        one_libs.extend(get_dependance_libs(modules_name[module][1]))
    else :
        one_libs.extend(get_dependance_libs(modules_name[module]))


    #those module need database
    if(module in module_databases.keys()) :
        create_database(module_databases[module])

    return one_libs;


def deploy_all_need_libs(libs) :
    sub_need_libs = []

    all_module_libs = need_libs.keys()
    cd_dir(deploy_root_dir + lib_dir)
    for lb in libs : 
        if lb in all_module_libs :
            mud = need_libs[lb][0]
            ret = get_dependance_libs(mud)
	    for item in ret :
		if(item not in all_module_libs) :
		    sub_need_libs.append(item)
	else :
	    print "Lib : " , lib , " not existing..."
	    sys.exit(-1)

    libs.extend(sub_need_libs)
    
    cstore_lib_path = cstore_deploy_path + cstore_lib
    execute_cmd("mkdir -p " + cstore_lib_path)
    for lb in libs  :
        execute_cmd("cp " + need_libs[lb][0] + " " + cstore_lib_path)


def deploy_modules(choose) :
    all_need_deploy_libs = []

    cd_dir(deploy_root_dir + module_dir)
    for module in choose :
        all_need_deploy_libs.extend(deploy_one_module(module))
    
    libpath = deploy_root_dir + lib_dir
    cd_dir(libpath)
    deploy_all_need_libs(all_need_deploy_libs)

    if(os.system("echo '" + libpath + "'" + " >> " + ldconfigpath) != 0) :
	print "echo " , libpath , " append to " , ldconfigpath , " error ..."
	sys.exit()
    execute_cmd("ldconfig")


import pdb
if __name__ == "__main__" :
    msg = '''Deploy none module ... choose a module : 
        cs for module cs
        mu for module mu 
        su for mudule su
        gw for module gateway
        ca for module ca
        all for module all'''
	
#    pdb.set_trace() 
    argc = len(sys.argv) 
    if(argc < 2) :
        print msg
        sys.exit(-1)
    
    choose = []
    for ch in range(1 , argc) :
        lch = sys.argv[ch].strip().lower()
        if lch not in modules :
            print lch
            print msg
            sys.exit(-1)

        choose.append(lch)

    if("all" in choose) :
        choose = list(modules)
        choose.remove("all")
    
    print "Choose deploy modules : " , str(choose)

    file = "deploy.tar"
    if(decompress(file) == -1) :
        print "Decompress file %s failed !" % file
        sys.exit(-1)
    else :
        print "Decompress file %s Success !"  % file

    deploy_modules(choose)

