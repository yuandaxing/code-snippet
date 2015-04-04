AS      =as
LD      =ld
CC      =g++
AR      =ar



CFLAGS =  -g -Wall -DMUTEX -DDEBUG -fno-strict-aliasing -std=c++11
CFLAGS += -I./
SRCS =  file_lock.cc  locker_test.cc
OBJS := $(SRCS:.cc=.o)
LDFLAGS = -lgtest -lpthread

EXECUT_TAGS := test.exe

all:$(EXECUT_TAGS) $(OBJS)


${EXECUT_TAGS}:${OBJS}
	$(CC) -o $@ $^ $(LDFLAGS) 

$(OBJS):%.o:%.cc
	$(CC) -c $(CFLAGS) -o $@ $<

.SUFFIXES:
.SUFFIXES: .cpp .o

.PHONY:clean
clean:
	$(RM) *.o
	$(RM) ${EXECUT_TAGS}
