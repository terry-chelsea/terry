/*
 * =============================================================================
 *
 *       Filename:  readXml.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011-11-02 16:00:54
 *  Last Modified:  2011-11-02 16:00:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =============================================================================
 */

#include "readXml.h"

#include <cstdlib>
#include <string>

#include <sys/types.h>
#include "XMLConfig.h"
/*
#include <xml/XML.h>

#include "log/log.h"
#include "util/util.h"
*/
int ReadXml::
configWithXML(const std::string &confFileName)
{
    XMLConfig *pXml = new XMLConfig("/var/pcp/config/module/ds/DSConfig.xml");

    std::string servport,mpcport,contport,sucrport,subaport,surbport,muport,rsport;

     
    pXml->getFirstNodeValue("/DS/port/servport", servport);
    pXml->getFirstNodeValue("/DS/port/mpcport",  mpcport);
    pXml->getFirstNodeValue("/DS/port/contport", contport);
    pXml->getFirstNodeValue("/DS/port/sucrport", sucrport);
    pXml->getFirstNodeValue("/DS/port/subaport", subaport);
    pXml->getFirstNodeValue("/DS/port/surbport", surbport);
    pXml->getFirstNodeValue("/DS/port/muport", muport);
    pXml->getFirstNodeValue("/DS/port/rsport", rsport);
    
    
 
    serv_port =(uint16_t)(atoi(servport.c_str()));
    mpc_port = (uint16_t)(atoi(mpcport.c_str()));
    cont_port =(uint16_t)(atoi(contport.c_str()));
    sucr_port =(uint16_t)(atoi(sucrport.c_str()));
    suba_port =(uint16_t)(atoi(subaport.c_str()));
    surb_port =(uint16_t)(atoi(surbport.c_str()));
    mu_port =  (uint16_t)(atoi(muport.c_str()));
    rs_port =  (uint16_t)(atoi(rsport.c_str()));

     
    pXml->getFirstNodeValue("/DS/db/sqlip",sqlip);
    pXml->getFirstNodeValue("/DS/db/sqluser",sqluser);
    pXml->getFirstNodeValue("/DS/db/sqlpwd",sqlpwd);
    pXml->getFirstNodeValue("/DS/db/sqldbname",sqldbname);


    
    return 0;
}
