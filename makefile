.POSIX :

RM = /bin/rm

TARGET = test
OBJS = test.o

all : $(TARGET)

clean :
	@-$(RM) $(TARGET) $(OBJS)

test.cpp : clog.h nomagic.h
