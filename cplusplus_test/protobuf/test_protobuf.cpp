/*
 * =====================================================================================
 *
 *       Filename:  test_protobuf.cpp
 *
 *    Description:  test protobuf , encode and decode...
 *
 *        Version:  1.0
 *        Created:  06/20/13 23:53:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include "test.pb.h"

using namespace std;

int encode_person_to_string(uint32_t age , uint32_t weigh , 
        const string &name , const string &team , string &encode)
{
    terry::person per;
    per.set_age(age);
    per.set_weigh(weigh);
    per.set_name(name);
    per.set_team(team);

    if(!per.IsInitialized() || !per.SerializeToString(&encode))
    {
        cerr<<"Serialize to string failed..."<<endl;
        return -1;
    }

    return 0;
}

int decode_person(const string &decode , uint32_t &age , uint32_t &weigh , 
        string &name , string &team)
{
    terry::person per;
    if(!per.ParseFromString(decode) || !per.IsInitialized())
    {
        cerr<<"parse from string failed..."<<endl;
        return -1;
    }


    age = per.age();
    weigh = per.weigh();
    name = per.name();
    team = per.team();

    return 0;
}

int main()
{
    string name("terry");
    string team("chelsea");
    uint32_t age = 32;
    uint32_t weigh = 82;

    string enc;
    if(encode_person_to_string(age , weigh , name , team , enc) < 0)
    {
        cerr<<"encode person to string failed..."<<endl;
        return -1;
    }
    else 
    {
        cout<<"encode success..."<<endl;
        cerr<<"name : "<<name<<endl;
        cerr<<"team : "<<team<<endl;
        cerr<<"age : "<<age<<endl;
        cerr<<"weigh : "<<weigh<<endl;

        cerr<<"after encode string length : "<<enc.size()<<endl;
    }

    uint32_t age_ , weigh_;
    string name_ , team_;
    if(decode_person(enc , age_ , weigh_ , name_ , team_) < 0)
    {
        cerr<<"decode person from string failed..."<<endl;
        return -1;
    }
    else 
    {
        cout<<"decode success..."<<endl;
        cerr<<"name : "<<name_<<endl;
        cerr<<"team : "<<team_<<endl;
        cerr<<"age : "<<age_<<endl;
        cerr<<"weigh : "<<weigh_<<endl;
    }

    return 0;
}
