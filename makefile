.POSIX :

FIND = /usr/bin/find
PWD = /bin/pwd
RM = /bin/rm

CC = gcc
LD = gcc
CFLAGS = -Wall -std=c++0x -pedantic $(INCPATH)
LDFLAGS = -lstdc++
ROOTDIR = $$($(PWD))
INCPATH = -I$(ROOTDIR) -I$(ROOTDIR)/otherlibs

TARGET = test
OBJS = main.o test.o
SRCS = $(OBJS:.o=.cpp)
DEPENDOPT = -MM

DIRS = an_impl

CALL_MAKE = 1
MAKEFILE = makefile
MAKEFILELISTR = -f $(MAKEFILE) -f depend
MAKEFILELISTC = -f $(ROOTDIR)/$(MAKEFILE) $(MAKEFILELISTR)

all : $(TARGET)

clean :
	@r=$(ROOTDIR) &&	\
		for i in $(DIRS);	\
		do	\
			(cd $$i &&	\
			$(MAKE) -f $${r}/$(MAKEFILE) -f $(MAKEFILE)	\
				"ROOTDIR=$$r" clean);	\
		done
	@-$(RM) $(TARGET) $(OBJS) depend

depend :
	@r=$(ROOTDIR) &&	\
		for i in $(DIRS);	\
		do	\
			(cd $$i &&	\
			$(MAKE) -f $${r}/$(MAKEFILE) -f $(MAKEFILE)	\
				"ROOTDIR=$$r" depend);	\
		done
	@-$(RM) depend
	@$(CC) $(CFLAGS) $(SRCS) $(DEPENDOPT) > depend

compile :
	@$(MAKE) -f $(ROOTDIR)/$(MAKEFILE) -f $(MAKEFILE) -f depend	\
			"ROOTDIR=$(ROOTDIR)"	\
			"CALL_MAKE=0" $(OBJS)

$(TARGET) : $(OBJS)
	@r=$(ROOTDIR) &&	\
		for i in $(DIRS);	\
		do	\
			(cd $$i &&	\
			$(MAKE) -f $${r}/$(MAKEFILE) -f $(MAKEFILE)	\
				"ROOTDIR=$$r" compile);	\
		done
	@if [ $(CALL_MAKE) -eq 1 ];	\
	then	\
		$(MAKE) -f $(MAKEFILE) -f depend	\
			"CALL_MAKE=0"	\
			"ROOTDIR=$(ROOTDIR)" $(TARGET);	\
	else	\
		$(LD) -o $@	\
			$$($(FIND) . -name '*.o') $(LDFLAGS);	\
	fi

.cpp.o :
	@if [ $(CALL_MAKE) -ne 1 ];	\
	then	\
		$(CC) -c -o $@ $(CFLAGS) $<;	\
	elif [ "x$$($(PWD))/$(MAKEFILE)" = "x$(ROOTDIR)/$(MAKEFILE)" ];	\
	then	\
		$(MAKE) $(MAKEFILELISTR)	\
			"CALL_MAKE=0"	\
			"ROOTDIR=$(ROOTDIR)" $<;	\
	else	\
		$(MAKE) $(MAKEFILELISTC)	\
			"CALL_MAKE=0"	\
			"ROOTDIR=$(ROOTDIR)" $<;	\
	fi


