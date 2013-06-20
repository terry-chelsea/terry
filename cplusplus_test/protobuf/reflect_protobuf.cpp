/*
 * =====================================================================================
 *
 *       Filename:  reflect_protobuf.cpp
 *
 *    Description:  replection of protobuf...
 *
 *        Version:  1.0
 *        Created:  06/21/13 00:19:20
 *       Revision:  none
 *       Compiler:  gcc
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <google/protobuf/descriptor.h>
#include "test.pb.h"

using namespace std;
using namespace google::protobuf;

Message *get_message_from_name(const string &name)
{
    const Descriptor *desp = DescriptorPool::generated_pool()->FindMessageTypeByName(name);
    if(desp)
    {
        const Message *msg = MessageFactory::generated_factory()->GetPrototype(desp);
        if(msg)
        {
            Message *handle = msg->New();
            return handle;
        }
    }

    return NULL;
}

int main()
{
    typedef terry::team  T;
    
    //从一个message的类型能够得到它的名称...
    string name = T::descriptor()->full_name();
    cerr<<"message name : "<<name<<endl;

    //根据名称可以得到只message对应的descriptor
    const Descriptor *desp = DescriptorPool::generated_pool()->FindMessageTypeByName(name);

    cerr<<"find message type by name : "<<desp<<endl;
    cerr<<"T::descriptor() : "<<T::descriptor()<<endl;

    //再根据descriptor得到包含它的message
    const Message *proto = MessageFactory::generated_factory()->GetPrototype(desp);

    cerr<<"get prototype : "<<proto<<endl;
    cerr<<"T:default_instance() : "<<&T::default_instance()<<endl;

    //最后通过New方法得到该类型的对象，所谓的prototype模式？
    T *ch = dynamic_cast<T *>(proto->New());

    cerr<<"prototype->New() : "<<ch<<endl;

    delete ch;


    Message *team = get_message_from_name("terry.team");
    if(NULL == team)
    {
        cerr<<"get message handle from name failed..."<<endl;
        return -1;
    }
    else 
        cerr<<"get message handler : "<<team<<endl;

    T *tm = dynamic_cast<T *>(team);
    if(tm != NULL)
        cerr<<"derived handler : "<<tm<<endl;
    else 
        cerr<<"prototype failed ..."<<endl;

    delete tm;
    return 0;
}
