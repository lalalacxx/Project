//先include系统的头文件
//再include库中的头文件
//最后写自己的
#include<stdio.h>
#include<def.h>
#include<sys/timerfd.h>
#include<string.h>
#include<stdint.h>
#include<unistd.h>
#include<sys/select.h>
#include<keyboard.h>

void draw_element(int x,int y,int c)
{
    x*=2;
    printf("\033[%d;%dH\033[?25l",y,x);
    printf("\033[3%dm\033[4%dm[]",c,c);
    fflush(stdout);
    printf("\033[0m");
}

//x = X+列   y=Y+行
void draw_shape(int x,int y,struct shape p,int c)
{
    int i = 0;
    int j = 0;
    for(i = 0;i < 5;i++)
    {
        for(j = 0;j < 5;j++)
        {
            if(p.s[i][j] != 0)
            {
                draw_element(x+j,y+i,c);
            }
        }
    }
}
void timer_tetris(struct data *p)
{
    draw_shape(p->x,p->y,shape_arr[0],BC);
    p->x++;
    p->y++;
    draw_shape(p->x,p->y,shape_arr[0],FC);
}
void tetris(struct data *p)
{
    int ret = 0;
    int c = get_key();
    if(is_up(c))
    {
        draw_shape(p->x,p->y,shape_arr[0],BC);
        p->y--;
        draw_shape(p->x,p->y,shape)arr[0],FC);
    }
    if(is_down(c))
    {
        draw_shape(p->x,p->y,shape_arr[0],BC);
        p->y++;
        draw_shape(p->x,p->y,shape)arr[0],FC);
    }
    if(is_left(c))
    {
        draw_shape(p->x,p->y,shape_arr[0],BC);
        p->x--;
        draw_shape(p->x,p->y,shape)arr[0],FC);
    }
    if(is_right(c))
    {
        draw_shape(p->x,p->y,shape_arr[0],BC);
        p->x++;
        draw_shape(p->x,p->y,shape)arr[0],FC);
    }
    if(is_esc(c))
    {
        ret = 1;
    }
    return ret;
}



int main()
{
    init_keyboard();
    //创建一个定时器
    int tfd = timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
    struct itimerspec ts;
    memset(&ts,0x00,sizeof(ts));
    ts.it_value.tv_nsec = 1;
    ts.it_interval.tv_sec = 1;
    timerfd_settime(tfd,0,&ts,NULL);

    fd_set rset;
    int kfd = fileno(stdin);
    int maxfd = tfd > kfd ? tfd :kfd;

    while(1)
    {
        FD_ZERO(&rset);
        FD_SET(tfd,&rset);
        FD_SET(kfd,&rset);
        int n = select(maxfd+1,&rset,NULL,NULL,NULL);
        //监控定时器
        if(FD_ISSET(tfd,&rset))
        {
            uint64_t clk;
            read(tfd,&clk,sizeof(clk));
            timer_tetris(&t);
        }
        //监控键盘
        if(FD_ISSET(kfd,&rset))
        {
            tetris(&t);
        }
    }
    printf("\033[?25h");
    recover_keyboard();
    return 0;
}
