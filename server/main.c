#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <mainproto.h>

#include "medialib.h"
#include "thrChnData.h"
#include "thrChnlist.h"

#define SIZE 1024

int serverSd;

int socketInit(void)
{
	struct ip_mreqn imq;

	serverSd = socket(AF_INET, SOCK_DGRAM, 0);
	if(-1 == serverSd){
		perror("socket()");
		exit(1);
	}

	inet_aton(MULTICASTIP, &imq.imr_multiaddr);
	inet_aton("0.0.0.0", &imq.imr_address);
	imq.imr_ifindex = if_nametoindex("ens33");
	if(setsockopt(serverSd, IPPROTO_IP, IP_MULTICAST_IF, &imq, sizeof(imq)) == -1){
		perror("setsockopt()");
		close(serverSd);
		return -1;
	}

	return 0;
}

int main(void)
{
	mlibChnList_t *mlib;
	int chnCnt;
	int i;
	char buf[BUFSIZE] ={};

	socketInit();

	mlibGetChnlist(&mlib, &chnCnt);

	thrChnListStart(mlib, chnCnt); 
/*
	recvfrom(serverSd, chnid, 10, 0, NULL, NULL);
	id = atoi(chnid);
	printf("%d\n", id);
*/


	for(i=1; i<=chnCnt; i++){
		fflush(NULL);
		thrChnDataRead(i);
}

	while(1)
		pause();

	pthread_exit(NULL);
}







#if 0
	strcpy(chnEntry.dscr, calloc(chnCnt, BUFSIZE));
	memcpy(chnList.list, calloc(chnCnt, sizeof(chnList)), chnCnt * sizeof(chnList));

	//memcpy(chnEntry, calloc(chnCnt, sizeof(struct listEntry_st)), chnCnt * sizeof(struct listEntry_st));

		for(i=0; i<chnCnt; i++){
			chnEntry.chnId = mlibArr[i].chnid;
			strcpy(chnEntry.dscr, mlibArr[i].descr);
			chnEntry.len = sizeof(chnEntry);
			memcpy(&chnList.list[i], &chnEntry, sizeof(struct listEntry_st));
			free(&chnEntry);
		}
/*
	while(1){
	 	if(sendto(sd, &chnList, sizeof(chnList), 0, (void *)&raddr, sizeof(raddr)) == -1){
			perror("sendto()");
			exit(1);
		}
		sleep(1);
	}
*/
	close(sd);

		for(i=0; i<chnCnt; i++){
			printf("%d  %s\n", chnList.list[i].chnId, \
chnList.list[i].dscr);
		}
	

	exit(0);
}
#endif 
