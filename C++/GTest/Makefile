AS      =as
LD      =ld
CC      =g++
AR      =ar


CFLAGS =  -g -Wall -DMUTEX -DDEBUG -fno-strict-aliasing
CFLAGS += -I./
LDFLAGS = -lgtest -lpthread
SRCS =  test.cpp
OBJS := $(SRCS:.cpp=.o)

EXECUT_TAGS := test.exe

all:$(EXECUT_TAGS) $(OBJS)


${EXECUT_TAGS}:${OBJS}
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJS):%.o:%.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

.SUFFIXES:
.SUFFIXES: .cpp .o

.PHONY:clean
clean:
	$(RM) *.o
	$(RM) ${EXECUT_TAGS}
