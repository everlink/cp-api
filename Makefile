OBJ=
CPBIN=el-client el-send

OPTIMIZATION=-O3
WARNINGS=-Wall -W -Wstrict-prototypes -Wwrite-strings
DEBUG= -g -ggdb
REAL_CFLAGS=$(OPTIMIZATION) -fPIC $(CFLAGS) $(WARNINGS) $(DEBUG) $(ARCH)
REAL_LDFLAGS=$(LDFLAGS) $(ARCH)


all: $(CPBIN)

query.o: query.c payload.h

el-client.o: el-client.c payload.h

el-send.o: el-send.c payload.h

el-client: el-client.o query.o
	$(CC) -o $@ $(REAL_CFLAGS) $(REAL_LDFLAGS) el-client.o query.o

el-send: el-send.o query.o
	$(CC) -o $@ $(REAL_CFLAGS) $(REAL_LDFLAGS) el-send.o query.o

cpbin: $(CPBIN)

.c.o:
	$(CC) -std=c99 -pedantic -c $(REAL_CFLAGS) $<

clean:
	rm -rf $(CPBIN) *.o

