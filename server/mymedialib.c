#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <glob.h>
#include <pthread.h>
#include <mainproto.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "medialib.h"


typedef struct fileData_t{
	int8_t chnid;
	char *descr;
	glob_t filePath;
}Data_t;

typedef struct{
	char *cont;
	size_t size;
	int chnnr;
	int *pathid;
	int num;
}arg_t;

static Data_t *job;
extern int serverSd;

	
static char * pathCm(char *buf, char *path, char *new)
{

	memset(buf, '\0', BUFSIZE);
	memcpy(buf, path, BUFSIZE);
	strcat(buf, "/");
	strcat(buf, new);
}

static int ReadFile(Data_t **t, int *chnCnt, mlibChnList_t **mlibArr)
{
	glob_t g;
	int i, j, tmp;
	char buf[BUFSIZE] = {};
	char *new;
	FILE *fp;
	char *ptr =  NULL;
	size_t n = 0;

	pathCm(buf, MLIBPATH, "*");
	if(glob(buf, GLOB_NOSORT, NULL, &g)!= 0){
		fprintf(stderr, "[%d][%s] glob failed\n", __LINE__, __FUNCTION__);
		return -1;
	}

	(*chnCnt) = g.gl_pathc;
	job = *t = calloc((*chnCnt), sizeof(Data_t));
	
	for(int i=0; i<g.gl_pathc; i++){
		pathCm(buf, g.gl_pathv[i], "*");
		if(glob(buf, GLOB_NOSORT, NULL, &((*t)[i].filePath)) != 0){
			perror("glob()");
			break;
		}
	}
	for(i=0; i<(*chnCnt); i++){
		(*t)[i].chnid = i+1;
		tmp = i;
		for(j=0; j<(*t)[tmp].filePath.gl_pathc; j++){
			if(strstr((*t)[tmp].filePath.gl_pathv[j], "descr") != NULL){
				fp = fopen((*t)[tmp].filePath.gl_pathv[j], "r");
				getline(&ptr, &n, fp);
				(*t)[tmp].descr = ptr;
				ptr = NULL;
			}
		}
	}


	(*mlibArr) = calloc((*chnCnt), sizeof(mlibChnList_t));
	for(i=0; i<(*chnCnt); i++){
		(*mlibArr)[i].chnid = (*t)[i].chnid;
		(*mlibArr)[i].descr = strdup((*t)[i].descr);
	}
	return 0;
}

int mlibGetChnlist(mlibChnList_t **mlibArr, int *chnCnt)
{
	int i;
	Data_t *t = NULL;
	
	ReadFile(&t, chnCnt, mlibArr);

	return 0;	
}

//读mp3线程

static char * readFile(arg_t *r)
{
	int fd[r->num];
	int cnt;
	int i;
	char *buf;
	int len = 0;

	r->cont = malloc(MSGSIZE);
	memset(r->cont, '\0', MSGSIZE);
	buf = (char *)malloc(MSGSIZE);
	memset(buf, '\0', MSGSIZE);

	for(i=0; i<(r->num); i++){
		//printf("mlib:%d\n", i);
		fd[i]=open((job[r->chnnr-1].filePath.gl_pathv)[(r->pathid)[i]], O_RDONLY);

		while(1){
			cnt = read(fd[i], r->cont, r->size);
			if(cnt == -1){
				perror("read()");
				close(fd[i]);
			}
			if(cnt == 0)
				break;
		}
		strcpy(buf+len, r->cont);
		len += strlen(r->cont); 
	}
	//puts(buf);
	return buf;
}

char * mlibReadChnData(int8_t chnid, void *buf, size_t size)
{
	int i;
	arg_t *arg;
	int err;
	char *msg;
	
	arg = malloc(sizeof(arg_t));
	arg->size = size;
	arg->num = 0;
	arg->pathid = malloc(10);

	for(i=0; i<job[chnid-1].filePath.gl_pathc; i++){
		if(strstr((job[chnid-1].filePath.gl_pathv)[i], ".mp3") != NULL){
			arg->chnnr=chnid;
			(arg->pathid)[arg->num]=i;
			(arg->num)++;
		}
	}

	msg = malloc(MSGSIZE);

	memcpy(msg, readFile(arg), MSGSIZE);
	//puts(msg);
	
	return msg;
}







#if 0

struct contex_st{
	char * descr;
	int8_t chnid;
	glob_t Filepath;
	int musicIndex;
	int pos;
}

static int parseChnEntry(const char *path, struct context_st *context)
{
	char buf[BUFSIZE] = {};
	FILE *fp;
	char *ptr = NULL;
	size_t n = 0;
	static int id = 1;

	strcpy(buf, path);
	strcat(buf, "/descr.txt");

	fp = fopen(buf, "r");
	if(fp == NULL){
		perror("fopen()");
		return -1;
	}
	getline(&ptr, &n, fp);
	context->descr = ptr;

	memset(buf, '\0', BUFSIZE);
	strcpy(buf, path);
	strcat(buf, "/*.mp3");
	glob(buf, GLOB_NOSORT, NULL, &context->Filepath);
	if((context->Filepath).gl_pathc == 0){
		free(ptr);
		return -1;
	}

	context->chnid = id++;
	context->pos = 0;
	context->musicIndex = 0;
	
	return 0
}
static int parseChnDir(struct context_st **mlibContext, int *chnnr)
{
	struct context_st *p = NULL;
	char buf[BUFSIZE] = {};
	glob_t res;
	int i;
	//FILE * fp;

	strcpy(buf, MLIBPATH);
	strcat(buf, "\*");

	if(glob(buf, GLOB_NOSORT, NULL, &res) != 0){
		fprintf(stderr, "[%d][%s]glob() failed\n", __LINE__, __FUNCTION__);
		return -1;
	}

	p = calloc(res.gl_pathc, sizeof(struct context_st));

	int validChnId = 0;

	for(i = 0; i < res.gl_pathc, i++){
		if(parseChnEntry((res.gl_pathv)[i], p + validChnIndex) == -1){
			continue;
		}
		validChnIndex++;
	}

	if(valibChnIndex != res.gl_pathc){
		p = realloc(p, validChnIndex * sizeof(struct context_st));
	*mlibContext = p;
	*chnnr = validChnIndex;
}

int mlibGetChnList(mlibChnList_t **mlibArr, int *chnCnt)
{
	struct context_st *p = NULL;
	int chnnr;
	int i;

	parseChnDir(&p, &chnnr);

	*mlibArr = calloc(chnnr, sizeof(mlibChnList_t));
	
	for(i = 0; i < chnnr; i++){
		(*mlibArr)[i].chnid = p[i].chnid;
		(*mlibArr)[i].descr = strdup(p[i].descr);
	}

	*chnCnt = chnnr;
	return 0;
}

#endif 
