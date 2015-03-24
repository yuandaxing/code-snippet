AS      =as
LD      =ld
CC      =g++
AR      =ar



CFLAGS =  -g -Wall -DMUTEX -DDEBUG -fno-strict-aliasing -std=c++11
CFLAGS += -I./
LDFLAGS = -lgtest -lmicrohttpd -lpthread
SRCS =  serv.cc serv_ut.cc request_mgr.cc  param_repository.cc
OBJS := $(SRCS:.cc=.o)

EXECUT_TAGS := test.exe

all:$(EXECUT_TAGS) $(OBJS)


${EXECUT_TAGS}:${OBJS}
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJS):%.o:%.cc
	$(CC) -c $(CFLAGS) -o $@ $<

.SUFFIXES:
.SUFFIXES: .cc .o

.PHONY:clean
clean:
	$(RM) *.o
	$(RM) ${EXECUT_TAGS}
