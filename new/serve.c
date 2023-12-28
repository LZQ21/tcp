#include "serve.h"
int fld; // 代表用户套接字存储结构体位置

int main()
{
    pthread_t pid = 0;
    time_t tm;
    char buf[BUFSIZE] = "";
    struct sockaddr_in saddr = {0}, caddr = {0};
    int size = sizeof(caddr);

    // 初始化。将文件中已存数据复原
    init();

    // 1.建立套接字
    sid = socket(AF_INET, SOCK_STREAM, 0);
    // 结构体参数配置

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 2.bind绑定
    if (bind(sid, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        perror("绑定失败\n");
        return -1;
    }
    printf("绑定成功\n");
    // 3.listen聆听
    int lis = listen(sid, MAXMEM);

    printf("正在等待连接\n");

    // 4.创建子线程用于退出
    pthread_create(&pid, NULL, (void *)(&quit), NULL);
    // 5.等待接受accpet
    //  cid = accept(sid,(struct sockaddr*)&caddr,&size);
    while (1)
    {
        printf("等待接受\n");
        int j;
        // cid = accept(sid,NULL,NULL);
        cid = accept(sid, (struct sockaddr *)&caddr, &size);

        // printf("cid = %d\n",cid);
        if (cid < 0)
        {
            printf("accept失败\n");
            break;
        }

        for (j = 0; j < MAXMEM; j++)
        {
            if (cidnum[j] == -1)
            {
                break; // 如果找到空闲位置-将cid存入该数组——注意先等待连接后加入！！！
            }
        }
        fld = j;
        printf("新一次创建进程下标j = %d\n",j);

        cidnum[fld] = cid;
        printf("accept成功\n");
        printf("fld = %d\n", fld);
        printf("cid = %d\n", cidnum[fld]);
        // 6.创建线程处理套接字文件
        printf("%p\n", &fld);
        pthread_create(&pid, NULL, (void *)(&snd_rec), &fld);
    }
}

void quit(void) // 这个线程一直在等待执行
{
    char tem[BUFSIZE] = "";
    while (1)
    {
        fgets(tem, sizeof(tem), stdin);
        if (!strcmp(tem, "quit\n"))
        {
            printf("再见\n");
            save_user();
            close(sid);
            exit(0);
        }
    }
}
void snd_rec(void *arg)
{
    printf("子线程\n");
    int i = *(int *)arg ; // 将i作为参数传递进来
    printf("i = %d\n", i);
    int len;
    char buf[BUFSIZE] = "";
    char temp[BUFSIZE] = "";
    char command[20], arg1[20], arg2[BUFSIZE];
    printf("i =  %d\n", i);
    printf(" cidnum[%d] = %d\n",i,cidnum[i]);

        while (1)
        {
            bzero(buf, sizeof(buf));
            len = recv(cidnum[i], buf, sizeof(buf), 0);
            printf("buf = %s\n", buf); // 开始从客户端读到数据

            if (len > 0)
            {
                buf[len - 1] = '\0'; // 用结束符\0取代换行符\n
                printf("i = %d\n", i);
                printf("buf:%s\n", buf);
                printf("进入判断截断\n");
                if (!strcmp(buf, "lo"))
                {
                    int a = login_in(i);
                    if (a == 0) // 登入成功，跳出本次循环
                    {
                        break;
                    }
                }
                else if (!strcmp(buf, "loc"))
                {
                    login_create(i);
                }
                else if (!strcmp(buf, "quit"))
                {
                    printf("客户端发出断开请求\n");
                    quit_client(i);
                }
                else
                {
                    printf("你暂时未登陆系统\n");
                }
            }
        }
        
        while (1)
        {
            printf("进入聊天系统\n");
            strcpy(buf, "请选择你接下来将要进行操作\n");
            send(cidnum[i], buf, strlen(buf) , 0);
            bzero(temp, sizeof(temp));
            len = recv(cidnum[i], temp, sizeof(temp) , 0);
            bzero(command,sizeof(command));
            bzero(arg1,sizeof(arg1));
            bzero(arg2,sizeof(arg2));

            sscanf(temp, "%s %s %[^\n]", command, arg1, arg2);
            printf("%s\t%s\t%s\n", command, arg1, arg2);

            if (len > 0)
            {
                temp[len - 1] = '\0'; // 用结束符\0取代换行符\n
            
             if (!strcmp(command, "createchat")) // 创建聊天室
            {
                create_chatroom(arg1, arg2, i);
            }
            else if (!strcmp(command, "joinchat"))//加入聊天室
            {
                join_chatroom(arg1, arg2, i);
            }
            else if (!strcmp(command, "quitchat"))//退出聊天室
            {
                quit_chatroom(i);
            }

            else if (!strcmp(command, "sendall")) // 向所有在线人员发送
            {
                all_chat(arg1, i);
            }
            else if (!strcmp(command, "sendpri")) // 私发
            {
                private_chat(arg1, arg2, i);
            }
            else if (!strcmp(command, "sendgroup"))//聊天室内发送
            {
                group_chat(arg1,arg2,i);
            }
            
            else if (!strcmp(command, "get") && (!strcmp(arg1, "all")))//获得已经登记所有人员数
            {
                get_all_users(i);
            }
            else if (!strcmp(command, "get") && (!strcmp(arg1, "online")))//获得在线所有人员数
            {
                get_online_users(i);
            }
             else if (!strcmp(command, "get") && (!strcmp(arg1, "chatmem")))//查询所在聊天室人数
            {
                get_chatroom_users(i);
            }
             else if (!strcmp(command, "get") && (!strcmp(arg1, "chat")))//获得聊天室数
            {
                get_chatroom(i);
            }
            else if (!strcmp(command, "quit"))
            {
                printf("客户端发出断开请求\n");
                quit_client(i);
            }
            else
            {
                strcpy(buf, "无效命令——请重新输入\n");
                send(cidnum[i], buf, strlen(buf), 0);
            }
                
            }
        }
    
    
}

void quit_client(int i)//执行完此函数后，服务端一直反复出现
{
    int x;
    printf(" cidnum[%d] = %d\n",i,cidnum[i]);
    close(cidnum[i]);
    printf(" cidnum[%d] = %d\n",i,cidnum[i]);
    printf("i = %d\n",i);
    for (x = 0; x < MAXROOM; x++)
    {
        if (i == online_users[x].sid)
        {
            online_users[x].status = -1; // 将此标记
            online_users[x].sid = -1;
        }
    }
    printf("i = %d\n",i);
    cidnum[i] = -1;
    printf(" cidnum[%d] = %d\n",i,cidnum[i]);
    
    pthread_exit((void *)0); // 关闭这个连接创建进程

    //close(cidnum[i]);
}
void join_chatroom(char *name, char *passwd, int i)
{
    
    char buf[BUFSIZE] = "";
    strcpy(buf, "你已经加入了此聊天室");
    send(cidnum[i], buf, sizeof(buf), 0);
    int a, b;
    int p, j;
    int flag = -1; // 代表人是否已经在聊天室
    int room = 0;  // 所在房间数组下标
    for (a = 0; a < MAXROOM; a++)
    {
        for (b = 0; b < MAXMEM; b++)
        {
            if (chatroom[a].user[b] == i)
            {
                flag = 0; // 代表已经在聊天室
                room = a; // 房间号
            }
        }
    }
    if (flag == 0)
    {
        strcpy(buf, "你已经加入了此聊天室");
        strcat(buf, "\n");
        strcat(buf, chatroom[room].name);
        send(cidnum[i], buf, sizeof(buf), 0);
    }
    else
        for (p = 0; p < MAXROOM; p++)
        {
            if (chatroom[p].status != -1) // 说明聊天室在线
            {
                if (strcmp(chatroom[p].name, name) == 0) // 聊天室相匹配
                {
                    if (!strcmp(chatroom[p].passwd, passwd))
                    {
                        for (j = 0; j < 10; j++) // 最多十个成员
                        {
                            if (chatroom[p].user[j] == -1)
                            {
                                break;
                            }
                        }
                        chatroom[p].user[j] = i; // 将套接字数组下标存进聊天室结构体

                        strcpy(buf, "成功加入了此聊天室");
                        send(cidnum[i], buf, sizeof(buf), 0);
                        return;
                        // 向群组成员发送消息
                    }
                }
            }
        }
}
void create_chatroom(char *name, char *passwd, int i)
{
    if ((name == NULL) || (passwd == NULL))
    {
        perror("参数不足\n");
        return;
    }
    
    int a, b;
    char buf[BUFSIZE] = "";
    for (a = 0; a < MAXROOM; a++)
    {
            if (chatroom[a].status == -1)
            break; // 如果status = -1 说明聊天室由空闲位置
    }
    strcpy(chatroom[chatroom_count].name, name);
    strcpy(chatroom[chatroom_count].passwd, passwd);
    chatroom[chatroom_count].status = 0; // 表示此聊天室已经有人并且在线
    chatroom_count++;//此参数用来计算创建聊天室数
    for (b = 0; b < MAXMEM; b++)
    {
        if (chatroom[a].user[b] == -1)
            break; // 如果status = -1 说明聊天室由空闲位置
    }
    printf("b= %d\n",b);
    chatroom[a].user[b] == i;
    strcpy(buf, "成功创建聊天室");
    strcat(buf,name);
    strcat(buf,"并且你已经加入");
    send(cidnum[i], buf, BUFSIZE, 0);
    return;
}

void private_chat(char *username, char *data, int i)
{
    int x, y;
    char buf[BUFSIZE] = "";
    char sendman[BUFSIZE] = "";
    char nowtime[BUFSIZE] = "";
    char temp[BUFSIZE] = ""; // 给私发发送人反馈
    time_t tm;
    tm = time(NULL);

    for (x = 0; x < MAXMEM; x++)
    {
        if (i == online_users[x].sid) // 
        {
            strcat(sendman, online_users[x].username); // 将发送人名赋值
            printf("你好\t%s",sendman);
            break;
        }
    }
    for (y = 0; y < user_count; y++) // 找私发目标人物
    {
        if (!strcmp(online_users[y].username, username))
        {
            strcat(buf, nowtime);
            strcat(buf, "\t");
            strcat(buf, "from");
            strcat(buf, sendman);
            strcat(buf, "\n");
            strcat(buf, data);
            send(cidnum[online_users[y].sid], buf, sizeof(buf), 0);

            strcat(temp, "成功发送");
            send(cidnum[i], temp, sizeof(temp), 0);
            return;
        }
        else
        {
            strcat(temp, "用户可能不在线,暂未找到该人物");
            send(cidnum[i], temp, sizeof(temp), 0);
            return;
        }
    }
}

void all_chat(char *tem, int i)

{
    int a;
    char buf[BUFSIZE] = "";
    char sendman[BUFSIZE] = "";
    time_t tm;
    char *timenow = NULL;
    time(&tm);
    timenow = ctime(&tm);

    for (int s = 0; s < MAXMEM; s++)
    {
        if (i == online_users[s].sid) // f发送的此人在线
        {
            strcpy(sendman, online_users[i].username);
            break;
        }
    }

    strcat(buf, timenow);
    strcat(buf, "\t");
    strcat(buf, "from");
    strcat(buf, "\t");
    strcat(buf, sendman);
    strcat(buf, ":");
    strcat(buf, tem);
    for (int a = 0; a < MAXMEM; a++)
    {
        if (a != i && cidnum[a] != -1) // 不发自己且其他人在线
        {
            send(cidnum[a], buf, strlen(buf), 0);
        }
    }

    strcpy(buf, "成功发送\n");
    send(cidnum[i], buf, strlen(buf), 0);

    
}
void group_chat(char *chatroomname, char *tem, int i)//聊天室发送
{
    int x,y;
    char buf[BUFSIZE] = "";
    //1.遍历找出名字匹配聊天室
    for (x = 0; x < chatroom_count; x++)
    {
        if (!strcmp(chatroomname,chatroom[x].name))
        {
            for(y = 0 ;y < 20;y++)
            {
                strcpy(buf,tem);
                if (chatroom[x].user[y] != -1)
                {
                   send(cidnum[chatroom[x].user[y]],buf,BUFSIZE,0);
                }
            }
        }
        
    }
    
    //2.对聊天室中所有人员sid进行信息发送
}
int login_in(int i)
{
    int len;
    int j;
    int k;
    // init();
    char buf[BUFSIZE], name[20], password[20];
    sprintf(buf, "登陆界面\n请输入你的名字");
    send(cidnum[i], buf, strlen(buf), 0);
    len = recv(cidnum[i], name, 20, 0);
    if (len > 0)
    {
        name[len - 1] = '\0';
    }
    sprintf(buf, "请输入你的密码");
    send(cidnum[i], buf, strlen(buf), 0);
    len = recv(cidnum[i], password, 20, 0);
    if (len > 0)
    {
        password[len - 1] = '\0';
    }

    for (j = 0; j < 20; j++)
    {
        if (strcmp(user[j].name, name) == 0)
        {
            if (strcmp(user[j].password, password) == 0)
            {
                for(int x = 0;x < MAXMEM ;x++)
                {
                    if(!strcmp(online_users[x].username,name))
                {
                     strcpy(buf, "你已经登陆\n");
                    send(cidnum[i], buf, strlen(buf) + 1, 0);
                    return -1;
                }
                    
                }

                strcpy(buf, "登陆成功\n");
                send(cidnum[i], buf, strlen(buf) + 1, 0);
                for (k = 0; k < MAXMEM; k++)
                {
                    if (online_users[k].status == -1) // 找空闲位置
                    {
                        break;
                    }
                }
                strcpy(online_users[k].username, name); // 将在线人员存入数组
                online_users[k].sid = i;
                online_users[k].status = 0;
                return 0;
            }
            else
            {
                strcpy(buf, "密码错误\n");
                send(cidnum[i], buf, strlen(buf) - 1, 0);
                return -1;
            }
        }
    }

    strcpy(buf, "群组此人未创建\n");
    send(cidnum[i], buf, strlen(buf) - 1, 0);
    return -1;
}
int login_create(int i) // 注册用户
{

    int len;
    int j;
    char buf[BUFSIZE], name[20], password[20];
    sprintf(buf, "创建账户\n请输入你的名字");
    send(cidnum[i], buf, strlen(buf), 0);
    printf("len = %d\n", len);
    len = recv(cidnum[i], name, 20, 0);
    if (len > 0)
    {
        name[len - 1] = '\0';
    }
    sprintf(buf, "请输入你的密码");

    send(cidnum[i], buf, strlen(buf), 0);
    len = recv(cidnum[i], password, 20, 0);
    if (len > 0)
    {
        password[len - 1] = '\0';
    }
    for (j = 0; j < user_count; j++)
    {
        if (strcmp(user[j].name, name) == 0)//如果名字相同，说明已经存在于用户数组中
        {
            strcpy(buf, "该昵称已经存在\n");
            send(cidnum[i], buf, strlen(buf) + 1, 0);
            return -1;
        }
    }
    printf("名字：%s\n", name);
    printf("密码：%s\n", password);
    printf("user_cout = %d\n", user_count);
    strcpy(user[user_count].name, name);
    strcpy(user[user_count].password, password);
    user_count++;//用户人数
    strcpy(buf, "创建成功\n");
    
    send(cidnum[i], buf, strlen(buf) + 1, 0);
    return 0;
}

void init(void)
{
    printf("初始化\n");

    while (fld < MAXMEM)
    {
        cidnum[fld] = -1;
        fld++; // 将套接字文件符清空//-1 表示空闲状态
    }

    int k, j;

    user_count = 0;     // 现在在线人数
    chatroom_count = 0; // 在线聊天室数
    for (k = 0; k < MAXMEM; k++)
    {
        online_users[k].status = -1;
        online_users[k].sid = -1;//在线人下标置-1
    }
    for (k = 0; k < MAXROOM; k++)
    {
        chatroom[k].status = -1;
        for (j = 0; j < MAXROOM; j++)
        {
            chatroom[k].user[j] = -1;
        }
    }

    char buf[BUFSIZE] = "";
    FILE *fp = NULL;
    fp = fopen("users.txt", "w+");
    if (fp == NULL)
    {
        perror("error\n");
        return;
    }

    while (fscanf(fp, "%s", buf) != EOF)
    {
        fwrite(buf, BUFSIZE, 1, fp);
        strcpy(user[user_count ].name, buf);

        fscanf(fp, "%s", buf); // 一个字符串，一个字符串的读写//从流中读取格式化输入
        strcpy(user[user_count ].name, buf);
        user_count++;
    }
    printf("初始化\n");
    fclose(fp);
}
void save_user()
{
    char buf[BUFSIZE] = "";
    FILE *fp = NULL;
    fp = fopen("users.txt", "w+");
    if(fp == NULL)
    {
        printf("文件打开失败\n");
    }

    for (fld = 0; fld < user_count; fld++)
    {
        strcpy(buf, user[fld].name); // 存储入会者姓名
        strcat(buf, "\n");
        fprintf(fp, "%s", buf); // 发送格式化buf到fp文件流
        strcpy(buf, user[fld].password);
        strcat(buf, "\n");
        fprintf(fp, "%s", buf);
    }
    fclose(fp);
}

void get_online_users(int i)
{
    char buf[BUFSIZE] = "";
    int x, y;
    strcat(buf, "全部在线人员");
    strcat(buf, "\n");
    for (x = 0; x < MAXMEM; x++)
    {
        if (online_users[x].status == 0)
        {
            strcat(buf, online_users[x].username);
            strcat(buf, "\n");
        }
    }
    send(cidnum[i], buf, strlen(buf), 0);
}

void get_all_users(int i)
{
    int x, y;
    char buf[BUFSIZE] = "";
    strcat(buf, "全部人员");
    strcat(buf, "\n");
    for (x = 0; x < user_count; x++)
    {
        strcat(buf, user[x].name);
        strcat(buf, "\n");
    }
    send(cidnum[i], buf, sizeof(buf), 0);
}
void get_chatroom(int i)
{
    int x;
    char buf[BUFSIZE] = "";
    strcat(buf, "全部聊天室");
    strcat(buf, "\n");
    for (x = 0; x < MAXROOM; x++)
    {
        if (chatroom[x].status == 0)
        {
            strcat(buf, chatroom[x].name);
            strcat(buf, "\n");
        }
    }
    send(cidnum[i], buf, sizeof(buf), 0);
}
void get_chatroom_users(int i)//获取所在群聊人数
{
    int x,y;
    int flag =-1;//作为所在聊天室下标，同时若发生变化，说明找到房间
    char buf[BUFSIZE] = "";
    for (x = 0; x < MAXMEM; x++)
    {
        if (online_users[x].sid = i)
        {
           strcpy(buf, online_users[x].username);
        }
        
    }
    
    strcat(buf, "所在聊天室人员有");
    strcat(buf, "\n"); 
     for (x = 0; x < MAXROOM; x++)//遍历找出所在聊天室
     {
        for ( y = 0; y < MAXMEM; y++)
        {
            if (chatroom[x].user[y] == i)
                {
                    flag = x;
                }
        }
     }
     if (flag == -1)
     {
           strcpy(buf,"你未加入此聊天室");
           send(cidnum[i], buf, sizeof(buf), 0);
           return;
     }
     else
     {
        for(x = 0;x < MAXMEM;x++)//将聊天室中所有人遍历出来
        {
            if (chatroom[flag].user[x] != -1)
            {
               for(y = 0; y < 20;y++)
               {
                if ((online_users[y].status != -1) && (chatroom[flag].user[x] == online_users[y].sid))
                {
                    strcat(buf,online_users[y].username);
                    strcat(buf,"\n");
                }
                
               }
            }
            
        }
        send(cidnum[i], buf, sizeof(buf), 0);
     }

     
}

void quit_chatroom(int i)//退出聊天室
{
    char buf[BUFSIZE] = "";
    int x ,y;
    
    
        for ( y = 0; y < MAXMEM; y++)
        {
            
            if (online_users[y].status == i)//找到此时退出的cid
            {
                strcpy(buf,online_users[y].username);
                strcat(buf,"退出聊天室");
                break;
            }
        } 
        for ( x = 0; x < MAXROOM; x++)
        {
            for ( y = 0; y < MAXMEM; y++)
            {

                if (chatroom[x].user[y] == i)//找到此时退出的cid
                {
                    chatroom[x].user[y] = -1;
                    strcat(buf,chatroom[x].name);
                    send(cidnum[i],buf,BUFSIZE,0);
                    break;
                }
            } 
        }
    
}