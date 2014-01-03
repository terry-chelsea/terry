/*
 * =============================================================================
 *
 *       Filename:  testIp.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011-10-26 10:12:34
 *  Last Modified:  2011-10-26 10:12:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =============================================================================
 */

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/vfs.h>
#include <sys/time.h>
#include <stdlib.h>
#include <net/if.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sstream>
#include <wait.h>

//#include "sysIn.h"
#include "readXml.h"
using namespace std;

#define msg(one,two) cout << one<< "........>" << two <<"_______"<< endl;

int 
main()
{
    ReadXml rd;


    string st("WDCongfig.xml");
    rd.configWithXML(st);
    
    msg("serv_port",  rd.serv_port);

    msg("mpcport",   rd.mpc_port);
    msg("contport",  rd.cont_port);
    msg("sucrport",  rd.sucr_port);
    msg("subaport",  rd.suba_port);
    msg("surbport",  rd.surb_port);
    msg("muport",    rd.mu_port);
    msg("rsport",    rd.rs_port);

    msg("sqlip",  rd.sqlip);
    msg("sqluser",  rd.sqluser);
    
    msg("sqlpwd",  rd.sqlpwd);
    msg("sqldbname",  rd.sqldbname);

    //msg("",  rd.);
    //msg("",  rd.);







}
