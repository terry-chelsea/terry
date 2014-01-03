/*
 * =====================================================================================
 *
 *       Filename:  readXml.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011-11-02 15:45:37
 *  Last Modified:  2011-11-02 15:45:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */
#ifndef _READXML_H_
#define _READXML_H_

#include <string>
#include <sys/types.h>
#include <inttypes.h>
//#include "sys/sys.h"
//#include "sysIn.h"
class ReadXml
{
    public:

        uint16_t serv_port;
        uint16_t mpc_port;
        uint16_t cont_port;
        uint16_t sucr_port;
        uint16_t suba_port;
        uint16_t surb_port;
        uint16_t rs_port;
        uint16_t mu_port;

        std::string sqlip;
        std::string sqluser;
        std::string sqlpwd;
        std::string sqldbname;
     

        //std::string xmlwdpath;

    public:
        int configWithXML(const std::string &configFileName);


    private:
        //ReadXml(){}
};
#endif // _READXML_H_
