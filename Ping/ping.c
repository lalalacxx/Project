#include <stdio.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>
//#include<unistd.h>
//校验和的算法
//addr:需要计算的校验和的数据的起始地址
//len:数据大小，单位是字节
unsigned short chksum(unsigned short *addr,int len)
{
    unsigned int ret = 0;
    while(len>1)
    {
        ret += *addr;
        addr++;
        len -= 2;
    }
    if(len == 1)
    {
        ret += *(unsigned char*)addr;
    }
    ret = (ret>>16) + (ret&0xffff);
    ret += (ret>>16);
    return (unsigned short)~ret;
}
//计算两个时间的时间差
float diftime(struct timeval *end,struct timeval *begin)
{
    float ret = (end->tv_sec - begin->tv_sec)*1000.0 + \
                (end->tv_usec - begin->tv_usec)/1000.0;
    return ret;
}


int main(int argc,char *argv[])
{
    if(argc != 2)
    {
        fprintf(stderr,"usage:./ping ip/域名\n");
        exit(1);
    }
    struct sockaddr_in addr;
    struct hostent *phost;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    if(addr.sin_addr.s_addr == INADDR_NONE)//ip解析出错
    {
        //ip不对，再判断是不是域名，当做域名进行解析
        phost = gethostbyname(argv[1]);//该函数返回的是一个结构体
        if(phost == NULL)//域名解析出错
        {
            perror("gethostbyname");
            exit(1);
        }
        memcpy((void*)&addr.sin_addr,phost->h_addr,phost->h_length);
        
    }
    return 0;
}
