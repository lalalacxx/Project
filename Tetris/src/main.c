#include <stdio.h>
#include <def.h>
#include <sys/timerfd.h>
#include <string.h>
#include <stdint.h>
#include <sys/select.h>
#include <unistd.h>
#include <keyboard.h>
#include <tetris.h>
#include <time.h>
#include <stdlib.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>

int init_socket()
{
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    int op = 1;
    setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&op,sizeof(op));
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_aton("127.0.0.1",&addr.sin_addr);
    int r = bind(lfd,(struct sockaddr*)&addr,sizeof(addr));
    if(r == 1)
    {
        perror("bind");
        exit(1);
    }
    listen(lfd,SOMAXCONN);
    return lfd;
}

int main( void )
{
	srand(time(NULL));
	init_keyboard();
    int lfd = init_socket();
	int tfd = timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
	struct itimerspec ts;
	memset(&ts, 0x00, sizeof ts);
	ts.it_value.tv_nsec = 1;
	ts.it_interval.tv_sec = 1;
	timerfd_settime(tfd, 0, &ts, NULL);
	
	fd_set rset;
	
	int kfd = fileno(stdin);
	int maxfd = tfd > kfd ? tfd : kfd;

	draw_back();

	next = rand()%7;
	show_next();
	while ( 1 ) {
		FD_ZERO(&rset);
		FD_SET(tfd, &rset);
		FD_SET(kfd, &rset);
		int n = select(maxfd+1, &rset, NULL, NULL, NULL);
		if ( FD_ISSET(tfd, &rset) ) {
			uint64_t clk;
			read(tfd, &clk, sizeof clk);
			timer_tetris(&t);
		}
		if ( FD_ISSET(kfd, &rset) ) {
			if ( tetris(&t) == 1 )
				break;
		}
		//if ( FD_ISSET(tfd, &rset) ) {
			//uint64_t clk;
			//read(tfd, &clk, sizeof clk);
			//timer_tetris(&t);
		}
	}

	printf("\033[?25h");
	recover_keyboard();
}

