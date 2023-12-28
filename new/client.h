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
#include<signal.h>
#include<poll.h>


#define PORT 10000
#define  IP "192.168.1.205"
#define BUFSIZE 128

int cid;
void snd();
void get_help();