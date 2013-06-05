
#include "Client.h"
#include "TCPAgent.h"
#include "AgentManager.h"
#include "Epoll.h"
#include "debug.h"
#include "BaseReq.h"
#include "SocketAddress.h"

#include "protocol.h"
#include "ca_login.pb.h"

#include <string>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

using namespace std;

struct timeval all_start , all_end;
Client **all_clients = NULL;
struct timeval *starts = NULL;
struct timeval *tm_ends = NULL;
double *Time_gaps = NULL;
unsigned int gap_index = 0;
unsigned int ALL_REQS = 0;
unsigned int users = 0;
unsigned int REQ_PER_USER = 0;
unsigned int LAST_USER = 0;
unsigned int nr = 0;

typedef double tagElemType;
void shellSort(void *pArray , size_t n)
{
    if(n < 2 || NULL == pArray)
        return ;

    tagElemType *array = (tagElemType *)pArray;
    size_t begin = n / 2;
    while(begin > 0)
    {
        size_t i = 0;
        size_t k = 0;
        size_t j = 0;
        for(k = 0 ; k < begin ; ++k)
        {
            for(i = k + begin ; i < n ; i += begin)
            {
                tagElemType temp = array[i];
                for(j = i ; j > k; j -= begin)
                {
                    if(temp > array[j - begin])
                        break;
                    else
                        array[j] = array[j - begin];
                }
                array[j] = temp;
            }
        }
        begin = begin / 2;
    }
}

#define PRINT_COUNTER 5000

void add_sum_time()
{
    double sum = 0;
    /*  
    uint32_t i = 0;
    for(i = 0 ; i < gap_index ; ++ i)
    {
        sum += Time_gaps[i];
    }
    */
    sum = ((all_end.tv_sec - all_start.tv_sec) * 1000000 + (all_end.tv_usec - all_start.tv_usec)) / 1000;

    cerr<<"Cost all time : "<<sum / 1000<<"s"<<endl;
    cerr<<"Requests per second : "<<(ALL_REQS * 1000) / (sum)<<endl;
    cerr<<"Time per request : "<<(sum) / (ALL_REQS)<<" ms"<<endl;
    cerr<<"Concurrency Level : "<<users<<endl;
    cerr<<"Time waiting per request : "<<(sum) / (ALL_REQS / users)<<endl;
}

void sort_all_time_and_display()
{
    cerr<<"Request get ack time : "<<endl;
    add_sum_time();
    shellSort(Time_gaps , gap_index);
    uint32_t i = 0;

    cerr<<"Most fast ack : "<<Time_gaps[0]<<" ms"<<endl;
    for(i = 10 ; i < 100 ; i += 10)
    {
        cerr<<i<<"% : "<<Time_gaps[gap_index * i / 100] / users<<" ms"<<endl;
    }
    cerr<<"100% : "<<Time_gaps[gap_index - 1] / users<<" ms"<<endl;
}

void check_times(double gap)
{
    if((gap_index + 1) % PRINT_COUNTER == 0)
        cerr<<"Completed "<<gap_index + 1<<" requests , connections "<<nr<<endl;

    Time_gaps[gap_index] = gap;
    gap_index ++;

    if(gap_index == ALL_REQS)
    {
        gettimeofday(&all_end , NULL);
        sort_all_time_and_display();
        exit(0);
    }
}

#define PRINT_TIME_INTERVAL(start , end) do { \
    double gap = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec); \
    LOG_INFO("Time interval is %.3lfms" , gap / 1000); \
    check_times(gap / 1000); \
}while(0)

//static uint32_t SUM = 0;
const char *emp_file = "user_names";
static char *CONTENT = NULL;
static int current_cur = 0;
vector<string> all_name;

struct Token get_login_ack(string in_data);
string generate_login_req(std::string name );

void deal_with_result(int con , int nr , uint32_t err)
{
    std::string info;
    switch (err)
    {
        case CA_WORKS_OK :
            info = "CA works well...";
            break;
        case CA_INSIDE_ERROR :
            info = "some error happened in CA ...";
            break;
        case UNDEFINED_CMD_TYPE :
            info = "Client send undefined command type...";
            break;
        case USER_NAME_ILLEGAL :
            info = "User name is illegal...";
            break;
        case USER_NAME_NOT_EXIST :
            info = "User is not exist ...";
            break;
        case USER_PASSWORD_DISMATCH :
            info = "User and pwssword dimatch...";
            break;
        case TOKEN_NOT_EXIST :
            info = "Token is not exist , please login...";
            break;
        case TOKEN_NEED_UPDATE :
            info = "Token is out of date , update is right now...";
            break;
        case PROTOBUF_ERROR :
            info = "Protobuf is error somewhere ...";
            break; 
        case CONNECT_MU_ERROR :
            info = "Connect MU error , can not happened !";
            break;
        case TOKEN_IS_ILLEGAL :
            info = "Token type is not legal ...";
            break;

        default :
            info = "Undefined error code , check it !";
            break;
    }

    if(err != CA_WORKS_OK)
        LOG_ERROR("Get connection %d request %d error info : %s" , con , nr , info.c_str());
    else 
        LOG_INFO("Get connection %d request %d error info : %s" , con , nr , info.c_str());

}

struct Token 
{
    string name;
    string password;
    long long uid;
};

int ca_connect_after(const Context &ctx)
{
    bool ret = ctx.para.result;
    TCPAgent *agent = ctx.agent;

    if(!ret)
    {
        LOG_ERROR("Connect to server error !");
        return -1;
    }

    LOG_INFO("Start time counter for client %d..." , nr);
#ifdef COUNT_CONNECTION_TIME
    gettimeofday(starts + nr , NULL);
    double gap = (starts[nr].tv_sec - tm_ends[nr].tv_sec) * 1000000
        + (starts[nr].tv_usec - tm_ends[nr].tv_usec);
    cerr<<"Connection "<<nr + 1<<" cost "<<gap / 1000<<" ms"<<endl;
#endif

    string name = all_name[nr];;
    string out_data = generate_login_req(name);
    agent->send_data(MSG_SYS_CA_USER_LOGIN , 0 , nr , 0 , out_data);

    ++nr;
    if(users == nr)
    {
        cerr<<"All connections has been using..."<<endl;
    }
    return 0;
}

int ca_recycler_after(const Context &ctx)
{
    LOG_INFO("Recycler for agent %p " , ctx.agent);
    return 0;
}

void ca_read_back(const Context &ctx)
{
//    TCPAgent *agent = ctx.agent;
    MsgHeader head = ctx.para.req_in->m_msgHeader;
    TCPAgent *agent = ctx.agent;

    string in_data;
    if(ctx.para.req_in->ioBuf != NULL)
        in_data.assign(ctx.para.req_in->ioBuf , head.length);

//    uint32_t para1 = head.para1;
    uint32_t para2 = head.para2;
    uint32_t para1 = head.para1;

    deal_with_result(para1 , para2 , head.error);

    gettimeofday(tm_ends + para1 , NULL);
    PRINT_TIME_INTERVAL(starts[para1] , tm_ends[para1]);

    if(CA_WORKS_OK == head.error)
    {
        struct Token token = get_login_ack(in_data);
//        time_t expi = time(NULL) + token.expire;
        LOG_INFO("name : %s password : %s and uid : %lld" , token.name.c_str() , token.password.c_str() , token.uid);
    }
    if(LAST_USER != 0)
    {
        if((para1 < LAST_USER && para2 == REQ_PER_USER) || (para1 >= LAST_USER && para2 == REQ_PER_USER - 1))
        {
            AgentManager::getInstancePtr()->recyclerAgent(agent);
            return ;
        }
    }
    else 
    {
        if(para2 == REQ_PER_USER - 1)
        {
            AgentManager::getInstancePtr()->recyclerAgent(agent);
            return ;
        }
    }

    string name = all_name[para1 * REQ_PER_USER + para2];
    string out_data = generate_login_req(name);
    agent->send_data(MSG_SYS_CA_USER_LOGIN , 0 , para1 , para2 + 1 , out_data);
    
    gettimeofday(starts+para1 , NULL);
}

void ca_write_back(const Context &ctx)
{
    bool ret = ctx.para.result;
    if(ret)
        LOG_INFO("send data success !");
    else 
        LOG_ERROR("Send data failed !");
}

const struct Agent_operations client_ops = 
{
    "CA client agent" , 
    ca_connect_after , 
    ca_recycler_after , 
    ca_read_back , 
    ca_write_back ,
};


#define temp_file "user_names"
#define DEFAULT_USERS 1
#define MAX_CLIENT  10000

int get_all_names();
int main(int argc , char *argv[])
{
    START_DEBUG(1 , "log_file");
    if(argc < 4)
    {
        LOG_ERROR("./Usage : IP port times [Users]!");
        return -1;
    }

    if(argv[3] != NULL)
        users = atoi(argv[3]);
    else 
        users = DEFAULT_USERS;

    char *ip = argv[1];
    unsigned int port = atoi(argv[2]);

    SocketAddress addr(ip , port);

    int fd = open(temp_file , O_RDONLY);
    if(fd < 0)
    {
        LOG_SYSCALL("open file %s error " , temp_file);
        return -1;
    }

    struct stat buf;
    if(fstat(fd , &buf) < 0)
    {
        LOG_SYSCALL("Stat file fd %d error !" , fd);
        return -1;
    }

    int len = buf.st_size;
    CONTENT = new char[len + 1];
    if(read(fd , CONTENT , len) != len)
    {
        LOG_SYSCALL("Read from file fd %d error !" , fd);
        return -1;
    }
    CONTENT[len] = '\0';
    current_cur = 0;
    close(fd);

    ALL_REQS = get_all_names();
    cerr<<"There are "<<users<<" connects and  requests "<<ALL_REQS<<" in sum..."<<endl;

    if(ALL_REQS % users == 0)
    {
        REQ_PER_USER = ALL_REQS / users;
        LAST_USER = 0;
        cerr<<"0--"<<users<<" each user send "<<REQ_PER_USER<<" requests"<<endl;
    }
    else 
    {
        REQ_PER_USER = ALL_REQS / users;
        LAST_USER = ALL_REQS - REQ_PER_USER * users;
        cerr<<"0--"<<LAST_USER - 1<<" each user send "<<REQ_PER_USER + 1<<" last "
        <<LAST_USER<<"--"<<users - 1<<" each user send "<<REQ_PER_USER
        <<" requests"<<endl;
    }

    Epoll *epl = new Epoll();
    if(epl->initialize(MAX_CLIENT) < 0)
    {
        LOG_ERROR("Epoll initialize error !");
        return -1;
    }

    all_clients = new Client *[users];
    starts = new struct timeval[users];
    tm_ends = new struct timeval[users];
    Time_gaps = new double[ALL_REQS];

    if(NULL == all_clients)
    {
        LOG_ERROR("Memory need ...");
        return -1;
    }

    Client *cli = NULL;
    for(unsigned int i = 0 ; i < users ; ++ i)
    {
        cli = new Client(epl);
        cli->set_operations(client_ops);
        if(cli->connect(addr) < 0)
        {
            LOG_ERROR("Client connect error !");
            return -1;
        }
#ifdef COUNT_CONNECTION_TIME
        gettimeofday(tm_ends + i , NULL);
#endif

        all_clients[i] = cli;
    }

    gettimeofday(&all_start , NULL);

    epl->run();

    return 0;
}


string generate_login_req(std::string name )
{
    CA::pb_MSG_SYS_CA_USER_LOGIN login;
    string out_data;

    login.set_name(name);
    
    login.SerializeToString(&out_data);

    return out_data;
}

struct Token get_login_ack(string in_data)
{
    CA::pb_MSG_SYS_CA_USER_LOGIN_ACK ack;
    ack.ParseFromString(in_data);

    struct Token tk;
    tk.name = ack.token();
    tk.password = ack.password();
    tk.uid = ack.user_id();

    return tk;
}

string get_one_user_name()
{
    char *start_ptr = CONTENT + current_cur;
    const char *start_cp = start_ptr;
    while((*start_ptr != '\n') && (*start_ptr != '\0'))
    {
        start_ptr ++;
    }

    string ret;
    if(*start_ptr == '\n')
    {
        int len = start_ptr - start_cp;
        ret.assign(start_cp , len);
        current_cur += len + 1;
    }
    else
        ret.clear();

    return ret;
}

int get_all_names()
{
    while(1)
    {
        string one = get_one_user_name();
        if(one.empty())
            break;

        all_name.push_back(one);
    }

    return all_name.size();
}
