#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<time.h>
#include<dirent.h>
#include<sys/wait.h>
#include<pthread.h>
#include<errno.h>
#include<semaphore.h>
#include<signal.h>
#include<sys/time.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/msg.h>
#include<sys/shm.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netdb.h>
#include<sys/select.h>
#include<poll.h>

#define PORT 10000
#define BUFSIZE 1024
#define MAXMEM 20
#define MAXROOM 5

int sid;
int cid;
int cidnum[MAXMEM] ;

int user_count;//
int chatroom_count;

struct  user
{
    char name[20];//用户姓名
    char password[20];//用户密码
};

struct user_socket
{
    char username[20];//姓名数组
    int sid;//套接字文件
    int status;//在线状态
};
struct chatroom
{
    char name[20] ;//聊天室姓名数据
    char passwd[20];//聊天组密码数据
    int user[10];//聊天组用户套接字
    int status;//聊天室存在状态标志位
};

struct user user[MAXMEM];
struct user_socket online_users[MAXMEM];
struct chatroom chatroom[MAXMEM];



void quit(void);//子线程结束服务端
void snd_rec(void *arg);//子线程接受发送
void quit_client(int i);//服务器关闭客户端
void private_chat(char *username, char *data,int i);//私发
void all_chat(char *tem, int i);//群发
void group_chat(char *chatroomname, char *tem, int i);//聊天室发送
int login_in(int i);//登陆账户
int login_create(int i);//创建账户
void init(void);//初始化
//退出登陆
//退出聊天室
void save_user();//保存数据

void create_chatroom(char *name ,char* passwd ,int i);//创建聊天室
void join_chatroom(char *name ,char *passwd ,int i);//加入聊天室
void quit_chatroom();//退出聊天室
void get_all_users(int i);//获取全部人数
void get_online_users(int i);//获取在线人数
void get_chatroom(int i);//获取群聊个数
void get_chatroom_users(int i);//获取所在群聊人数