#include<sys/time.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/ip_icmp.h>
#include<netdb.h>

#include<stdlib.h>
#include<string.h>
#include <stdio.h>

#define DATA_LEN 56 

int sendnum = 0;//发送数据包的编号
int recvnum = 0;//接受数据包的编号
char sendbuf[1024];//发送数据包
char recvbuf[1024];//接受数据包


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
//组包
//num:ICMP报文序号
//pid:组ICMP报文的标识符字段
///查看ICMP报头格式：usr/include/netinet/ip_icmp.h
int pack(int num,pid_t pid)
{
    struct icmp *picmp = (struct icmp*)sendbuf;
    picmp->icmp_type = ICMP_ECHO;
    picmp->icmp_code = 0;
    picmp->icmp_cksum = 0;
    picmp->icmp_id = pid;
    picmp->icmp_seq = htons(num);
    gettimeofday((struct timeval*)(picmp->icmp_data),NULL);//data的起始地址
    return DATA_LEN+8;
}
//发送数据包
//sfd:套接字描述符
//pid:组ICMP报文的标识符字段
//addr:发送的目标机器
void send_packet(int sfd,pid_t pid,struct sockaddr_in addr)
{
    sendnum++;
    int r = pack(sendnum,pid);

    sendto(sfd,sendbuf,r,0,(struct sockaddr*)&addr,sizeof(addr));
}
void recv_packet(int sfd,pid_t pid);
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
    printf("PING %s(%s) %d bytes of data.\n",argv[1],inet_ntoa(addr.sin_addr),DATA_LEN);
    
    int sfd = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
    if(sfd == -1)
    {
        perror("socket");
        exit(1);
    }
    pid_t pid = getpid();
    while(1)
    {
        send_packet(sfd,pid,addr);
        recv_packet(sfd,pid);
        sleep(1);
    }
    return 0;
}
