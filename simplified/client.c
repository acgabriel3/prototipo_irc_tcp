#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>

#ifndef ADDRESS
#define ADDRESS "127.0.0.1"
#endif
#ifndef PORT
#define PORT 5000
#endif

int
main()
{
	int ret = 0;
	int conn_fd;
	int maxfd;
	int nread;
	int nready;
	struct sockaddr_in server_addr = { 0 };
	char buf[1024];
	fd_set fds;

	server_addr.sin_family = AF_INET;

	server_addr.sin_port = htons(PORT);

	ret = inet_pton(AF_INET, ADDRESS, &server_addr.sin_addr);
	if (ret != 1) {
		if (ret == -1) {
			perror("inet_pton");
		}
		fprintf(stderr,
		        "failed to convert address %s "
		        "to binary net address\n",
		        ADDRESS);
		return -1;
	}

	fprintf(stdout, "CONNECTING: address=%s port=%d\n", ADDRESS, PORT);

	conn_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (conn_fd == -1) {
		perror("socket");
		return -1;
	}

	ret = connect(conn_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (ret == -1) {
		perror("connect");
		return -1;
	}

	fprintf(stdout, "Connected\n");

	maxfd = conn_fd + 1;
	while(1) {
		FD_ZERO(&fds);
		FD_SET(conn_fd, &fds);
		FD_SET(0, &fds);

		nready = select(maxfd, &fds, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);

		if(FD_ISSET(conn_fd, &fds)) {
			nread = recv(conn_fd, buf, 2000, 0);
			if(nread < 1)
			{
				fprintf(stdout, "Receive failed.");
				close(conn_fd);
				return -1;
			}
			write(1, buf, nread);
		}

		if(FD_ISSET(0, &fds)) {
			nread = read(0, buf, sizeof(buf));
			if(nread < 1) {
				close(conn_fd);
				return -1;
			}
			send(conn_fd, buf, nread, 0);
		}
	}

	ret = shutdown(conn_fd, SHUT_RDWR);
	if (ret == -1) {
		perror("shutdown");
		return -1;
	}

	ret = close(conn_fd);
	if (ret == -1) {
		perror("close");
		return -1;
	}

	return 0;
}
