#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<pthread.h>
#include<sys/sendfile.h>
#include<ctype.h>

#define MAX 1024
#define HOME_PAGE "index.html"

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
void clear_deal(int sock)
{
    char line[MAX];
    do
    {
        get_http_line(sock,line,sizeof(line));
    }
    while(strcmp(line,"\n") != 0);
}
void echo_www(int sock,char *path,int size,int *err)
{
    clear_deal(sock);
    //走到这里说明请求已经处理完了
    
    //把path响应回去
    int fd = open(path,O_RDONLY);
    if(fd < 0)
    {
        *err = 404;
        return;
    }
    char line[MAX];
    sprintf(line,"HTTP/1.0 200 OK\r\n");
    //发送状态行
    send(sock,line,strlen(line),0);
    //sprintf(line,"Content-Type:text/html;charset=ISO-8859-1");
    //send(sock,line,strlen(line),0);    
    sprintf(line,"\r\n");
    send(sock,line,strlen(line),0);
    //响应  fd往sock里写size个
    sendfile(sock,fd,NULL,size);
    
    close(fd);
}
void echo_error(int errcode)
{
    switch(errcode)
    {
        case 404:
            break;
        case 501:
            break;
        default:
            break;
    }
}
void *deal_request(void *arg)
{
    int sock = (int)arg;
    char line[MAX];
    char method[MAX/32];
    char url[MAX];
    char path[MAX];
    int errcode = 200;
    //cgi:通用网关接口。http服务内置的标准，方便后续扩展
    int cgi = 0;
    char *query_string = NULL;

#ifdef Debug
    do
    {
        get_http_line(sock,line,sizeof(line));
        printf("%s",line);
    }
    while(strcmp(line,"\n") != 0);
#else
    if(get_http_line(sock,line,sizeof(line)) < 0)
    {
        errcode = 404;
        goto end;
    }

    //获得请求方法method
    //   line[] = GET / HTTP/1.1
    int i = 0;//method下标
    int j = 0;//line下标

    //该循环用于提取请求方法
    while(i < sizeof(method)-1 && j < sizeof(line) && !isspace(line[j]))
    {
        method[i] = line[j];
        i++,j++;
    }
    method[i] = '\0';

    //strcasecmp()函数比较时忽略大小写
    if(strcasecmp(method,"GET") == 0)
    {

    }
    else if(strcasecmp(method,"POST") == 0)
    {
        cgi = 1;
    }
    else
    {
        errcode = 404;
        goto end;
    }

    //该循环结束，j指向下一个非空格
    while(j < sizeof(line) && isspace(line[j]))
    {
        j++;
    }

    //该循环用于提取url
    i = 0;
    while(i < sizeof(url)-1 && j < sizeof(line) && !isspace(line[j]))
    {
        url[i] = line[j];
        i++,j++;
    }
    url[i] = '\0';
    //分割url和query_string
    //url:请求的资源
    //query_string：给资源的参数
    if(strcasecmp(method,"GET") == 0)
    {
        query_string = url;
        while(*query_string)
        {
            if(*query_string == '?')
            {
                *query_string = '\0';
                query_string++;
                cgi = 1;
                break;
            }
            query_string++;
        }
    }
    
    // method:get或者post方法，cgi：0或者1
    // url：资源，query_string：空或者参数
    sprintf(path,"serverroot%s",url);
    if(path[strlen(path)-1] == '/')
    {
        //路径的最后一个是'/',此时将首页拼接上去
        strcat(path,HOME_PAGE);
    }

    //判断文件是否存在
    struct stat st;
    if(stat(path,&st) < 0)
    {
        //该文件不存在
        errcode = 404;
        goto end;
    }
    else
    {
        //判断访问的文件是否为一个目录
        if(S_ISDIR(st.st_mode))
        {
            strcat(path,HOME_PAGE);
        }
        else
        {
            //如果我们访问的文件具有可执行权限
            //则需要以cgi方式运行
            if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) \
               || (st.st_mode & S_IXOTH))
            {
                cgi = 1;
            }
        }
        if(cgi)
        {
            //一定是以cgi方式运行
            //exe_cgi();
        }
        else
        {
            //一定不以cgi方式运行，所以肯定是get方法，并且没有传参
            //要返回的就是一个普通文件
            //st_size:资源大小
            echo_www(sock,path,st.st_size,&errcode);
        }
    }
#endif
end:
    if(errcode != 200)
    {
        echo_error(errcode);
    }
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
