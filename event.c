#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdlib.h>
#include <string.h>

int fd;
struct input_event event;

void input_handler(int signum)
{
	while(read(fd, &event, sizeof(struct input_event)) > 0) {
		// printf("tv_sec:%d\n",event.time.tv_sec);
		// printf("tv_usec:%d\n",event.time.tv_usec);
		printf("type:%d\t",event.type);
		printf("code:%x\t",event.code);
		printf("value:%d\n",event.value);
		if(event.type==0 && event.code==2)
			printf("\n");
		else if(event.type==0&&event.code==0)
			printf("------------------------------\n");
	}
	return ;
}

int main(int argc,char *argv[])
{
	char *_path = "/dev/input/event3"; 
	int oflags;
	if(argc == 2 && argv[1]!=NULL) _path = argv[1];

	fd = open(_path, O_RDWR | O_NONBLOCK, S_IRUSR | S_IWUSR);
	if (fd >= 0) {
		signal(SIGIO, input_handler);
		fcntl(fd, F_SETOWN, getpid());
		oflags = fcntl(fd, F_GETFL);
		fcntl(fd, F_SETFL, oflags | FASYNC);
		while(1) {
			if( getchar() == 'q') {
				printf("Quit!");
				break;	
			}
		}
                close(fd);
	} else {
		printf("device open failure\n");
	}

	return 0;
}
