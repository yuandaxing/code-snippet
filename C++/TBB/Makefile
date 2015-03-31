AS      =as
LD      =ld
CC      =g++
AR      =ar



CFLAGS =  -g -Wall -DMUTEX -DDEBUG -fno-strict-aliasing
CFLAGS += -I./
LDFLAGS = -lgtest -lpthread -ltbb
SRCS =   queue_ut.cc hashmap_ut.cc
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
