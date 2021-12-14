#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <string.h>
#include <mainproto.h>

static int clientSd;

int sockeInit()
{
	struct ip_mreqn imq;
	struct sockaddr_in laddr;
	socklen_t addlen;

	clientSd = socket(AF_INET, SOCK_DGRAM, 0);
	if(-1 == clientSd){
		perror("socket()");
		return -1;
	}

	laddr.sin_family = AF_INET;
	inet_aton("0.0.0.0", &laddr.sin_addr);
	laddr.sin_port = htons(RCVPORT);
	addlen = sizeof(laddr);
	if(bind(clientSd, (void *)&laddr, addlen) == -1){
		perror("bind()");
		close(clientSd);
		return -1;
	}

	inet_aton(MULTICASTIP, &imq.imr_multiaddr);
	inet_aton("0.0.0.0", &imq.imr_address);
	imq.imr_ifindex = if_nametoindex("ens33");

	if(setsockopt(clientSd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&imq, sizeof(imq)) == -1){
		perror("setsockopt()");
		close(clientSd);
		return -1;
	}
}
/*
void channelChoose()
{
	int chnid;
	char buf[10] = {};

	printf("please select the channle you like:\n");
	scanf("%d", &chnid);
	sprintf(buf, "%d", chnid);

	while(1){

}
*/	

int main(void)
{
	struct list_st *list;
	struct listData_st *data;
	int8_t chnid;
	int len;
	struct listEntry_st *listEntry;
	pid_t pid;
	int fd[2];
	int cnt, cnt2;
	int f;

	sockeInit();

	list = malloc(MSGSIZE);
	data = malloc(MSGSIZE);

	while(1){
		len = recvfrom(clientSd, list, MSGSIZE, 0, NULL, NULL);
		
		if(len <0){
			perror("recvfrom()");
			goto ERROR;
		}
		if(list->chnId!=0)
			continue;
		break;
	}

	for(listEntry = (char *)list + sizeof(int8_t); \
listEntry < (char *)list +len; \
listEntry = (char *)listEntry + listEntry->len){

		printf("%d   %s\n", listEntry->chnId, listEntry->dscr);
		sleep(1);
	}
	
	printf("please choose the channle you like\n");
	scanf("%hhd", &chnid);
	
	if(pipe(fd) < 0){
		perror("pipe()");
		goto ERROR;
	}

	pid = fork();
	if(pid ==-1){
		perror("fork()");
		goto ERROR;
	}

	if(pid == 0){
		char buf[MSGSIZE]={};
		close(fd[1]);
		dup2(fd[0], 0);
/*
		while(1){
			memset(buf, '\0', MSGSIZE);
			cnt2 = read(fd[0], buf, MSGSIZE);
			f = open("/home/leslie/test1.mp3", O_CREAT | O_TRUNC | O_RDWR, 0666);
			write(f, buf, cnt2);
			//puts(buf);
		}
*/
		execl("/usr/bin/mplayer", "mplayer","-", NULL);
	}

	data = malloc(MSGSIZE);
	while(1){
		cnt = recvfrom(clientSd, data, MSGSIZE, 0, NULL, NULL);
		if(cnt == -1){
			perror("recvfrom()");
			goto ERROR;
		}
		if(data->chnId != chnid){
			continue;
		}
		//fflush(NULL);
		//printf("%d\n", cnt);
		//printf("chn:%d msg:%s\n", data->chnId, data->chnmsg);
		//fflush(NULL);
		write(fd[1], data->chnmsg, cnt-1);
	}

	exit(0);
ERROR:

	close(clientSd);
	exit(1);
}
