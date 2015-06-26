.POSIX :

RM = /bin/rm

CFLAGS = -Wall -std=c++0x -pedantic -I./
LDFLAGS = -lstdc++

TARGET = test
OBJS = main.o test.o outimpl.o outimpl_test.o
SRCS = $(OBJS:.o=.cpp)
DEPENDOPT = -MM

CALL_MAKE = 1
MAKEFILE = makefile

all : $(TARGET)

clean :
	@-$(RM) $(TARGET) $(OBJS) depend

depend :
	@-$(RM) depend
	@$(CC) $(CFLAGS) $(SRCS) $(DEPENDOPT) > depend

$(TARGET) : $(SRCS)
	@if [ $(CALL_MAKE) -eq 1 ];	\
	then	\
		$(MAKE) -f $(MAKEFILE) -f depend	\
			"CALL_MAKE=0" $(TARGET);	\
	else	\
		$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(SRCS);	\
	fi

