#ifndef __MEDIALIB_H
#define __MEDIALIB_H

#define MLIBPATH   "/home/leslie/mlib"

typedef struct {
	int8_t chnid;
	char *descr;
}mlibChnList_t;

int mlibGetChnlist(mlibChnList_t **mlibArr, int *chnCnt);

char *mlibReadChnData(int8_t chnid, void *buf, size_t size); 



#endif 
