#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "service.h"

static void usage(void);

int main(int argc, char *argv[]) {
	int sockfd, n;
	struct sockaddr_in srv_addr;
	char req[REQMAXLEN], rsp[RSPMAXLEN];
	const char *prompt = "msg: ";

	if (argc != 2)
		usage();
	if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		err(1, "tcpcli");
	memset(&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(TCP_SERVICE_PORT);
	srv_addr.sin_addr.s_addr = inet_addr(argv[1]);

	write(1, prompt, strlen(prompt));
	if ((n = read(1, req, REQMAXLEN)) < 0)
		err(1, "read");
	if (connect(sockfd, (SA) &srv_addr, sizeof(srv_addr)) == -1)
		err(1, "connect");
	if (write(sockfd, req, n) != n)
		err(1, "write");
	if ((n = read(sockfd, rsp, RSPMAXLEN)) < 0)
		err(1, "read");

	if (n > 0 && rsp[n-1] == '\n')
		rsp[n-1] = '\0';
	printf("response(%d): [%s]\n", n, rsp);

	exit(0);
}

static void
usage(void)
{
	fprintf(stderr, "usage: tcpcli <ip>");
	exit(1);
}
