#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<pthread.h>

#define MAX 1024

int startUp(int port)
{
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        //学习使用日志库，将错误消息输出到日志文件
        perror("socket");
        exit(2);
    }
    int opt = 1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    //绑定
    if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0)
    {
        perror("bind");
        exit(3);
    }
    //监听
    if(listen(sock,5) < 0)
    {
        perror("listen");
        exit(4);
    }
    return sock;
}
int get_http_line(int sock,char line[],int size)
{
    int c = 'c';
    int i = 0;
    ssize_t s = 0;
    while(i < size-1 && c != '\n')
    {
        s = recv(sock,&c,1,0);
        if(s > 0)
        {
            if(c =='\r')
            {
                //  '\r'->'\n'或者'\r\n'->'\n'
                //  MSG_PEEK:窥探下一个字符是否为'\n'
                //  并没有取走当前字符
                recv(sock,&c,1,MSG_PEEK);
                if(c != '\n')
                {
                    c = '\n';
                }
                else
                {
                    //拿走'\n'
                    recv(sock,&c,1,0);
                }
            }
            //  c == \n
            line[i++] = c;
        }
        else
        {
            //读失败了
            break;
        }
    }//while结束
    line[i] = '\0';
    return i;
}
void *deal_request(void *arg)
{
    int sock = (int)arg;
    char line[MAX];
    do
    {
        get_http_line(sock,line,sizeof(line));
        printf("%s",line);
    }
    while(strcmp(line,"\n") != 0);
    close(sock);
    return NULL;
}
//httpd 8080
int main(int argc,char *argv[])
{
    if(argc != 2)
    {
        printf("usage:%s [port]\n",argv[0]);
        return 1;
    }
    //创建监听套接字
    int listen_sock = startUp(atoi(argv[1]));
    for(;;)
    {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int new_sock = accept(listen_sock,(struct sockaddr*)&client,&len);
        if(new_sock < 0)
        {
            perror("accept");
            continue;
        }
        //走到这里说明连接获取成功
        pthread_t id;
        pthread_create(&id,NULL,deal_request,(void *)new_sock);
        pthread_detach(id);
    }
}























