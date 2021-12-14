LDFLAGS+=-pthread
CFLAGS+=-I/c/unix/netradio/proto
SRC=main.c mymedialib.c thrChnlist.c thrChnData.c

server:$(SRC)
		gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)
