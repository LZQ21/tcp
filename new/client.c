#include"client.h"
int sid;
int main()
{
    struct sockaddr_in saddr = {0};
    char buf[BUFSIZE] = "";
    pthread_t pid;
    
    //1.建立套接字
    sid = socket(AF_INET,SOCK_STREAM,0);
    if(sid < 0)
    {
        perror("socket errro\n");
        return -1;
    }
    //结构体参数配置
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    //saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(sid,IP,&saddr.sin_addr);
    //2.连接connect
    if(connect(sid,(struct sockaddr *)&saddr,sizeof(saddr)) < 0)
    {
        perror("connect failed\n");
        return -1;
    }
    printf("connect success\n");
    
    //3.创建子线程用于接受
    pthread_create(&pid,NULL,(void*)(&snd),NULL);
    //4，接受消息的线程
    while (1)
    {
        int len = 0;
        if((len = recv(sid,buf,sizeof(buf)-1,0)) > 0)
        {
            write(1,buf,len);
            printf("\n");
        }
    }
    
    return 0;
    
}
void snd()
{
    char buf[BUFSIZE] = "";
    while (1)
    {
        bzero(buf,sizeof(buf));
        fgets(buf,sizeof(buf),stdin);
        printf("buf =%s\n",buf);
       
       if (!strcmp(buf,"quit\n") )//退出客户端
       {
            printf("退出服务端\n");
            send(sid,buf,strlen(buf),0);
            exit(0);
       }
       else if (!strcmp(buf,"help\n"))//获取提示信息
       {
            get_help();
       }
       else//其他情况默认为发送到客户端
       {
            send(sid,buf,strlen(buf),0);
       }
    }
}
void get_help()
{
    printf("使用指南\n");
    printf("输入quit退出聊天\n");
    printf("输入help获取使用指南\n");
    printf("lo | 登陆账户\n");
    printf("loc|创建账户\n");
    printf("quit|退出客户端，断开连接\n");
    printf("createchat| 聊天室名|聊天室密码|创建聊天室\n");
    printf("joinchat|聊天室名|密码|加入聊天室\n");
    printf("quitchat|退出所在聊天室\n");
    printf("sendall|消息|向所有在线成员发送消息\n");
    printf("sendpri|用户名|消息|私发至某人\n");
    printf("sendgroup|向所在聊天室内所有人员发送消息\n");
    printf("get|all|获得所有注册用户信息\n");
    printf("get|online|获得在线所有人员信息\n");
    printf("get|chatmem|查询所在聊天室成员信息\n");
    printf("get|chat|获得所有聊天室信息 \n");
}

