AS      =as
LD      =ld
CC      =g++
AR      =ar


LD_PATH = -L../lib/
CFLAGS =  -g -Wall -DMUTEX -DDEBUG -fno-strict-aliasing -std=c++11
CFLAGS += -I./ -I../include/
LDFLAGS = -lgtest -lmicrohttpd -lpthread
SRCS =  serv.cc serv_ut.cc request_mgr.cc  param_repository.cc
OBJS := $(SRCS:.cc=.o)

EXECUT_TAGS := test.exe

all:$(EXECUT_TAGS) $(OBJS)


${EXECUT_TAGS}:${OBJS}
	$(CC) -o $@ $^ $(LD_PATH) $(LDFLAGS)

$(OBJS):%.o:%.cc
	$(CC) -c $(CFLAGS) -o $@ $<

.SUFFIXES:
.SUFFIXES: .cc .o

.PHONY:clean
clean:
	$(RM) *.o
	$(RM) ${EXECUT_TAGS}
