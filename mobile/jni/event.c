#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "log.h"

int fd;
struct input_event event;

void input_handler(int signum)
{
        static char ch = 'A';
        static struct input_event ev = {{0, 0}, 0, 0, 30};

	while(read(fd, &event, sizeof(struct input_event)) > 0) {
		// printf("tv_sec:%d\n",event.time.tv_sec);
		// printf("tv_usec:%d\n",event.time.tv_usec);
		printf("type:%d\t",event.type);
		printf("code:%x\t",event.code);
		printf("value:%d\n",event.value);
                if (event.value == 1 && event.type == 1) {
                        syslog(LOG_USER | LOG_INFO, "char:%c\tcode:%x\tvalue:%d", ch++, event.code, ev.value); 
                }
                ev.value = event.value;
		if(event.type==0 && event.code==2)
			printf("\n");
		else if(event.type==0&&event.code==0)
			printf("------------------------------\n");
	}
	return ;
}

int send_event(int fd)
{
        struct input_event ev;

        ev.type = 4;
        ev.code = 4;
        ev.value = 30;
        if (sizeof(ev) != write(fd, &ev, sizeof(ev))) {
                perror("fail to write event");
                return -1;
        }
        ev.type = 1;
        ev.code = 0x1e;
        ev.value = 1;
        if (sizeof(ev) != write(fd, &ev, sizeof(ev))) {
                perror("fail to write event");
                return -1;
        }
        ev.type = 0;
        ev.code = 0;
        ev.value = 0;
        if (sizeof(ev) != write(fd, &ev, sizeof(ev))) {
                perror("fail to write event");
                return -1;
        }
        ev.type = 4;
        ev.code = 4;
        ev.value = 30;
        if (sizeof(ev) != write(fd, &ev, sizeof(ev))) {
                perror("fail to write event");
                return -1;
        }
        ev.type = 1;
        ev.code = 0x1e;
        ev.value = 0;
        if (sizeof(ev) != write(fd, &ev, sizeof(ev))) {
                perror("fail to write event");
                return -1;
        }
        ev.type = 0;
        ev.code = 0;
        ev.value = 0;
        if (sizeof(ev) != write(fd, &ev, sizeof(ev))) {
                perror("fail to write event");
                return -1;
        }
        return 0;
}

/*
int main(int argc,char *argv[])
{
	char *_path = "/dev/input/event3"; 
	int oflags;
	if(argc == 2 && argv[1]) _path = argv[1];

	fd = open(_path, O_RDWR | O_NONBLOCK, S_IRUSR | S_IWUSR);
	if (fd >= 0) {
                init_log();
                signal(SIGIO, input_handler);
                fcntl(fd, F_SETOWN, getpid());
                oflags = fcntl(fd, F_GETFL);
                fcntl(fd, F_SETFL, oflags | FASYNC);
                //printf("start to do send_event\n");
                while(1) {
                        if(getchar() == 'q') {
                                printf("Quit!");
                                break;	
                        }
                }
                //send_event(fd);
                //printf("end to do send_event\n");
                close(fd);
	} else {
		perror("device open failure");
	}
        printf("\nThe end\n");

	return 0;
}
*/
