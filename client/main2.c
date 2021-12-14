#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "proto.h"

static void chnChoose(int n);
	

int main(void)
{
	int sd;
	struct sockaddr_in laddr;
	struct ip_mreqn imr;
	int num;
	struct listData_st rcvList;

	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd == -1){
		perror("socket()");
		exit(1);
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(SERVERPORT);
	inet_aton(SERVERIP, &laddr.sin_addr);
	if(bind(sd, (void *)&laddr, sizeof(laddr)) == -1){
		perror("bind()");
		goto ERROR;
	}

	inet_aton(MULTIIP, &imr.imr_multiaddr);
	inet_aton("0.0.0.0", &imr.imr_address);
	imr.imr_ifindex = if_nametoindex("ens33");
	if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr,\
sizeof(imr)) == -1){
		perror("setsockopt()");
		exit(1);
	}


	//scanf("%d", &num);
	//chnChoose(num);	
	if(recvfrom(sd, &rcvList, sizeof(rcvList), 0, NULL, NULL) == -1){
		perror("recvfrom()");
		exit(1);
	}

	for(int i=0; rcvList.list[i] != NULL; i++){
		printf("%d  %s", rcvList.list[i].chnId, \
rcvList.list[i].dscr);
	}

	exit(0);

ERROR:
	close(sd);
	exit(1);
}
/*
static void chnChoose(int n)
{
	switch(n){
		case 0:
			showList();
			break;
		case 1:
			playPopsong();
			break;
		case 2:
			playOpera();
			break;
		case 3:
			playTalkshow();
			break;
		default:
			break;
}
*/
