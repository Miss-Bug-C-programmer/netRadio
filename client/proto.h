#ifndef __MAINPROTO_H
#define __MAINPROTO_H

#define MULTICASTIP	"224.2.2.2"
#define RCVPORT		2233

#define CHNNR		200
#define CHNLISTID	0
#define MINCHNID	1
#define MAXCHNID	(MINCHNID)+(CHNNR)-1
#define MSGSIZE		1024

#define BUFSIZE	128
// 频道列表
// 每一个频道数据类型
struct listEntry_st {
	int8_t chnId;
	int8_t len; // 自述大小
	char dscr[1]; // 频道描述首地址
}__attribute__((packed)); 
// 频道列表整体数据类型
struct list_st {
	int8_t chnId; // 一定为0
	struct listEntry_st list[1];
}__attribute__((packed));

// 频道数据
struct listData_st {
	int8_t chnId;
	char chnmsg[MSGSIZE];
}__attribute__((packed));

#endif

