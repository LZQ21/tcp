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
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //inet_pton(sid,IP,&saddr.sin_addr);
    //2.连接connect
    if(connect(sid,(struct sockaddr *)&saddr,sizeof(saddr)) < 0)
    {
        perror("connect failed\n");
        return -1;
    }
    printf("connect success\n");
    //————————————————————————增加————————————————————————//
   //想监控几个文件描述符，数组个数就为几
	struct pollfd pda[2] = {0};
	//填写要监控的文件描述符和关心的事件
	//对于此文件描述符 输入事件
	pda[0].fd = sid; //套接字文件
	pda[0].events = POLLIN;
    pda[1].fd = STDIN_FILENO; //键盘输入文件
	pda[1].events = POLLIN;

	int count = 0;
    ///////////////////////////////////////////////////

    int len = 0 ;
    char nbuf[128] = "";//发送数据存放位置
    while (1)
    {
        count = poll(pda,2,1);
        if (count == 0)
        {
            // printf("没有发生你想的操作 count = %d\n",count );
        }
        if (pda[0].revents == POLLIN)//有socket变化接受到文件
        {
            printf("情况1接受\n");
            bzero(buf,sizeof(buf));
            
            if(len  < 0)
            {
                perror("recive error\n");
                return -1;
            }
            printf("ken %d\n",len);
            recv(sid,buf,sizeof(buf),0);
            
            printf("accept value is :%s\n",buf);
            count = 0;
        }
        
        if (pda[1].revents == POLLIN)//键盘有输入
        {
            printf("情况2发送\n");
            bzero(nbuf,sizeof(nbuf));
            read(STDIN_FILENO,nbuf,sizeof(nbuf)-1);
            printf("nbuf =%s\n",nbuf);
       
        if (!strcmp(nbuf,"quit\n") )//退出客户端
        {
            printf("退出服务端\n");
            char buf[BUFSIZE] = "";
            strcpy(buf,nbuf);
            send(sid,buf,strlen(buf),0);
            exit(0);

            //break;
           
        }
        else if (!strcmp(nbuf,"help\n"))//获取提示信息
        {
            printf("2\n");
                get_help();
        }
        else//其他情况默认为发送到客户端
        {
            printf("3\n");
                send(sid,nbuf,strlen(nbuf),0);
        }
                sleep (1);
                count = 0;
        }
            
        
    }
    return 0;
}
void get_help()
{
    printf("使用指南\n");
    printf("输入quit退出聊天\n");
    printf("输入help获取使用指南\n");

}