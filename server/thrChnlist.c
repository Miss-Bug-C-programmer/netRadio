#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <mainproto.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "thrChnlist.h"

static const mlibChnList_t *ml;
static int Cnt;
extern int serverSd;

static void *thrList(void *s)
{
	struct listEntry_st *entry;
	struct list_st *list;
	struct sockaddr_in grpAddr;
	int size = 0;
	int sum = 0;
	int i;

	list = malloc(MSGSIZE);
	if(list == NULL){
		perror("malloc()");
		return NULL;
	}
	list->chnId = 0;
	sum =  sizeof(list->chnId);

	for(i=0; i<Cnt; i++){
		size = sizeof(struct listEntry_st) + strlen(ml[i].descr);
		entry = malloc(size);
		entry->chnId = ml[i].chnid;
		strcpy(entry->dscr, ml[i].descr);
		entry->len = size;
		memcpy((char *)list+sum, entry, size);
		sum+=size;
		free(entry);
	}

	list =  realloc(list, sum);

	grpAddr.sin_family = AF_INET;
	inet_aton(MULTICASTIP, &grpAddr.sin_addr);
	grpAddr.sin_port = htons(RCVPORT);

	while(1){
		sendto(serverSd, list, sum, 0, (void *)&grpAddr, sizeof(grpAddr));
		sleep(1);
	}
}

		
int thrChnListStart(const mlibChnList_t *mlib, int chnCnt)
{
	ml = mlib;
	Cnt = chnCnt;
	pthread_t pt;
	int err;

	if((err = pthread_create(&pt, NULL, thrList, NULL)) != 0){
		fprintf(stderr, "pthread_create():%s\n", strerror(err));
		return -1;
	}

	pthread_detach(pt);
}
	
