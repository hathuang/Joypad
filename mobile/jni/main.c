#include <stdio.h>
#include <dirent.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/epoll.h>
//#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "io.h"
#include "protocol.h"

#define MAX_EPOLLFDS            4
#define MOBILE_ORT              30303

void client(void *arg)
{
        pthread_detach(pthread_self());
        int port = (int)arg;
        char buf[64], ch;
        int fd, ret;
        static int clientid = 0;
        int id;

        id = ++clientid;
        //sleep(1);
        printf("I am client, port is %d\n", port);
        fd = init_client_sock(port, "127.0.0.1");
        if (fd < 0) return ;
        ch = 0;
        while (1) {
                if (ch++ >= 25) ch = 0; 
                snprintf(buf, sizeof buf, "client = %d: %c <--> %c", id, 'A'+ch, 'a'+ch);
                ret = strlen(buf);
                if (ret != write(fd, buf, ret)) {
                        perror("fail to write to");
                        break;
                }
                if ((ret = read(fd, buf, sizeof(buf)-1)) < 0) {
                        perror("fail to read");
                        break;
                }
                buf[ret] = 0;
                printf("clientID = %2d  get : %s\n", id, buf);
                sleep(1);
        }
        close(fd);
}

int init_server_sock(int port) {
	int socket_fd;
	struct sockaddr_in server_addr;

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
	memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                perror("file to socket");
		return -1;
	}

	if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
                perror("file to bind");
		return -1;
	}

	if (listen(socket_fd, 8) < 0) {
                perror("file to listen");
		return -1;
	}
	
	return socket_fd;
}

int accept_client(int server_fd) {
	int client_fd, sin_size;
	struct sockaddr_in remote_addr; 
       
	sin_size = sizeof(remote_addr);
	if ((client_fd = accept(server_fd, (struct sockaddr *) &remote_addr, &sin_size)) < 0) {
		return -1;
	}

	return client_fd;
}

int init_client_sock(int port, const char *host) {
	int sock;
	struct sockaddr_in addr;
        
        if (!host) return -1; 
	memset(&addr, 0, sizeof(addr));
	if ((addr.sin_addr.s_addr = inet_addr(host)) == htonl(INADDR_NONE)) {
	        return -1;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		close(sock);
		return -1;
	}
	return sock;
}

int do_cmd(int fd, const char *cmd, const char *arg)
{
        int cmd_no;
        unsigned int arg_size, ret;
        char msg[1024];

        if (fd < 0 || !cmd) return -1;

        cmd_no = UNKOWN;
        arg_size = MSG2INT(cmd+CMD_SIZE);
        if (!strncmp(cmd, CMD_OKAY, CMD_SIZE)) {
                cmd_no = OKAY;
                // TODO
        } else if (!strncmp(cmd, CMD_BUSY, CMD_SIZE)) {
                cmd_no = BUSY;
                // TODO
        } else if (!strncmp(cmd, CMD_CNTL, CMD_SIZE)) {
                cmd_no = CNTL;
                // TODO
        } else if (!strncmp(cmd, CMD_SYNC, CMD_SIZE)) {
                cmd_no = SYNC;
                // TODO
        } else if (!strncmp(cmd, CMD_XCHG, CMD_SIZE)) {
                cmd_no = XCHG;
                // TODO
        } else if (!strncmp(cmd, CMD_FAIL, CMD_SIZE)) {
                cmd_no = FAIL;
                if (arg_size > sizeof(msg)-1 || arg_size == 0) {
                        fprintf(stderr, "ugly size of arg !");
                } else if ((ret = readx(fd, msg, arg_size)) != arg_size) {
                        fprintf(stderr, "fail to get enough size %d/%d bytes.", ret, arg_size);
                } else {
                        msg[arg_size] = 0;
                        fprintf(stderr, "fail msg : %s.", msg);
                }
        }

        return 0;
}

int main(int argc, char *argv[])
{
        int epoll_fd, nfds, fd, i, ret;
        //pthread_t client_thread;
        struct epoll_event ev,events[MAX_EPOLLFDS];
        char buf[1024]; 
        
        // get the event dev (from the conf file ?)
        // listen the 30303 port
        // accept client
        // session (read event and send it to client)
        
        int listen_fd = init_server_sock(MOBILE_PORT); // port = 30303

        //i = 1;
        //while (i--) {
        //if (pthread_create(&client_thread, NULL, (void *)client, (void *)port)) {
        //perror("fail to new a client thread");
        //return 0;
        //}
        //}

        epoll_fd = epoll_create(MAX_EPOLLFDS);
        if (epoll_fd < 0) {
                perror("fail to epoll_create");
                return 0;
        }

        ev.data.fd = listen_fd;
        ev.events = EPOLLIN | EPOLLET;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev)) {
                perror("fail to epoll_ctl");
                goto FAIL;
        }
#ifdef Debug
        printf("Start the session loop\n");
#endif
        while (1) {
                nfds = epoll_wait(epoll_fd, events, MAX_EPOLLFDS, 2000); // 2s
                for (i = 0; i < nfds; i++) {
                        if (events[i].data.fd == listen_fd) {
                                if ((fd = accept_client(listen_fd)) < 0) {
                                        perror("fail to accept_client");
                                        goto FAIL;
                                }
                                ev.data.fd = fd;
                                ev.events = EPOLLIN | EPOLLET;
                                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev)) {
                                        perror("fail to epoll_ctl");
                                        goto FAIL;
                                }
#ifdef Debug
                                fprintf(stderr, "Accept a client\n");
#endif
                        } else if (events[i].events & EPOLLIN) {
                                if ((fd = events[i].data.fd) < 0) {
                                        fprintf(stderr, "ugly fd\n");
                                        goto FAIL;
                                }
                                if ((ret = readx(fd, buf, MSG_SIZE)) < 0 || (ret && ret != MSG_SIZE)) {
                                        perror("fail to read");
                                        goto FAIL;
                                } else if (ret == 0) {
                                        close(fd);
                                        events[i].data.fd = -1;
                                }
#ifdef Debug
                                *(buf + ret) = 0;
                                fprintf(stderr, "server get : %s\n", buf);
#endif
                                if (do_cmd()) {
                                        
                                }
                                ev.data.fd = fd;
                                ev.events = EPOLLOUT | EPOLLET;
                                if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev)) {
                                        perror("fail to epoll_ctl");
                                        goto FAIL;
                                }
                        }  else if (events[i].events & EPOLLOUT) {
                                if ((fd = events[i].data.fd) < 0) {
                                        fprintf(stderr, "ugly fd\n");
                                        goto FAIL;
                                }
                                if (4 != write(fd, "ABCD", 4)) {
                                        perror("fail to write");
                                        goto FAIL;
                                }
                                ev.data.fd = fd;
                                ev.events = EPOLLIN | EPOLLET;
                                if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev)) {
                                        perror("fail to epoll_ctl");
                                        goto FAIL;
                                }
                        }
                }
        }
FAIL:
        close(listen_fd);
        fprintf(stderr, "The end\n");

        return 0;
}
