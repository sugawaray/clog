.POSIX :

CP = /bin/cp
ECHO = /bin/echo
FIND = /usr/bin/find
PWD = /bin/pwd
RM = /bin/rm
WC = /usr/bin/wc

CC = gcc
LD = gcc
CFLAGS = -Wall -std=c++0x -pedantic $(INCPATH)
LDFLAGS = -lstdc++ -lm
ROOTDIR = $$($(PWD))
INCPATH = -I$(ROOTDIR)/include -I$(ROOTDIR)/otherlibs
INCPATH_AN_IMPL = -I$(ROOTDIR)/include -I$(ROOTDIR)/otherlibs -I$(ROOTDIR)/src

TARGET = test
CLOGA = libclog.a
OBJS = 
SRCS = $(OBJS:.o=.cpp)
DEPENDOPT = -MM

DIRS = an_impl src

CALL_MAKE = 1
MAKEFILE = makefile
MAKEFILELISTR = -f $(MAKEFILE) -f depend
MAKEFILELISTC = -f $(ROOTDIR)/$(MAKEFILE) $(MAKEFILELISTR)

all : $(TARGET) create_cloga

clean :
	@r=$(ROOTDIR) &&	\
		for i in $(DIRS);	\
		do	\
			(cd $$i &&	\
			$(MAKE) -f $${r}/$(MAKEFILE) -f $(MAKEFILE)	\
				"ROOTDIR=$$r" clean);	\
		done
	@-$(RM) $(TARGET) $(OBJS) depend
	@-if [ "x$$($(PWD))" = "x$(ROOTDIR)" ];	\
	then	\
		$(RM) ./src/$(CLOGA);	\
		$(RM) ./lib/$(CLOGA);	\
	fi

depend :
	@r=$(ROOTDIR) &&	\
		for i in $(DIRS);	\
		do	\
			(cd $$i &&	\
			$(MAKE) -f $${r}/$(MAKEFILE) -f $(MAKEFILE)	\
				"ROOTDIR=$$r" depend);	\
		done
	@-$(RM) depend
	@if [ $$($(ECHO) $(SRCS) | $(WC) -w) -ne 0 ];	\
	then	\
		$(CC) $(CFLAGS) $(SRCS) $(DEPENDOPT) > depend;	\
	fi

compile :
	@$(MAKE) -f $(ROOTDIR)/$(MAKEFILE) -f $(MAKEFILE) -f depend	\
			"ROOTDIR=$(ROOTDIR)"	\
			"CALL_MAKE=0" $(OBJS)

$(TARGET) : $(OBJS) call_submake
	@if [ $(CALL_MAKE) -eq 1 ];	\
	then	\
		$(MAKE) -f $(MAKEFILE) -f depend	\
			"CALL_MAKE=0"	\
			"ROOTDIR=$(ROOTDIR)" $(TARGET);	\
	else	\
		$(LD) -o $@	\
			$$($(FIND) . -name '*.o') $(LDFLAGS);	\
	fi

create_cloga :
	@r=$(ROOTDIR) && (	\
		cd ./src &&	\
		$(MAKE) -f $${r}/$(MAKEFILE) -f $(MAKEFILE)	\
			"ROOTDIR=$$r" $(CLOGA)	\
		)
	@$(CP) ./src/$(CLOGA) ./lib

call_submake :
	@r=$(ROOTDIR) &&	\
		for i in $(DIRS);	\
		do	\
			(cd $$i &&	\
			$(MAKE) -f $${r}/$(MAKEFILE) -f $(MAKEFILE)	\
				"ROOTDIR=$$r" compile);	\
		done

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


