.POSIX :

RM = /bin/rm

CFLAGS = -Wall -std=c++0x -pedantic
LDFLAGS = -lstdc++

TARGET = test
OBJS = test.o
SRCS = $(OBJS:.o=.cpp)

all : $(TARGET)

clean :
	@-$(RM) $(TARGET) $(OBJS)

$(TARGET) : test.cpp
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(SRCS)

test.cpp : clog.h nomagic.h
