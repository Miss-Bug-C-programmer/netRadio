#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <mainproto.h>
#include <pthread.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "thrChnData.h"
#include "medialib.h"

extern int serverSd;

static void *thrData(void *data)
{
	struct sockaddr_in grpAddr;
	struct listData_st *d = malloc(MSGSIZE);
	d = (struct listData_st *)data;

	grpAddr.sin_family = AF_INET;
	inet_aton(MULTICASTIP,&grpAddr.sin_addr);
	grpAddr.sin_port = htons(RCVPORT);

	//puts(d->chnmsg);	
	
	while(1){
		sendto(serverSd, d, (strlen(d->chnmsg)+sizeof(int8_t)), 0, (void *)&grpAddr, sizeof(grpAddr));
		sleep(1);
	}
}
int thrChnDataRead(int8_t chnid)
{
	int i;
	pthread_t pt;
	struct listData_st *data;
	char buf[MSGSIZE] = {};
	char msg[MSGSIZE] = {};

	data = malloc(sizeof(struct listData_st)); 
	strcpy(msg, mlibReadChnData(chnid, buf, MSGSIZE));
	data->chnId = chnid;
	strcpy(data->chnmsg, msg);

	//printf("id:[%d], msg:%s\n", data->chnId, data->chnmsg);

	if((pthread_create(&pt, NULL, thrData, (void *)data))!=0){
		perror("pthread_create()");
		return -1;
	}
}
