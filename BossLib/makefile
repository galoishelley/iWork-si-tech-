#!/bin/sh
.SUFFIXES:.pc .cpp .cc .c .o

ORADIR=$(ORACLE_HOME)
WORKDIR=$(HOME)/run/public/BossLib
BINDIR=$(WORKDIR)/bin
LIBDIR=$(WORKDIR)/lib

ORAINCL1 = $(ORADIR)/rdbms/demo
ORAINCL2 = $(ORADIR)/rdbms/public
ORAINCL3 = $(ORADIR)/precomp/public
ORALIBS = $(ORADIR)/lib -lclntsh
TUXINCL = $(TUXDIR)/include
TUXLIBS = $(TUXDIR)/lib -ltux -lbuft -lfml -lfml32 -lwsc

#include = -I$(WORKDIR)/include -I$(TUXINCL) -I$(ORAINCL1) -I$(ORAINCL2) -I$(ORAINCL3) -I/usr/include -I/usr/include/sys -I/opt/aCC/include
INCLUDE = -I$(WORKDIR)/include -I$(PUBINCL) -I$(TUXINCL) -I$(ORAINCL3) -I$(ORAINCL2) -I$(ORAINCL1) -I/usr/include/sys -I/usr/include/aCC_std/rw
CFLAGS = -g

#CC=aCC +DD64 -AA
#CC=aCC +DD64  -Aa -DPLATFORM_TYPE=HP +Z
CC=aCC +DD64 
#-g -Ag++ -D_REENTRANT -D_USEDCI  -mt -w +z +Z   -DPLATFORM_TYPE=PLATFORM_TYPE_HP -D_LVSETENABLE -DMACHINE_WORD=64 -D__BIGMSGQUEUE_ENABLED -D__HP_NO_STRING_OVERLOADS -D__USE_XOPEN_EXTENDED -D_XOPEN_SOURCE_EXTENDED -D_INCLUDE_POSIX4_SOURCE -D_HPUX_SOURCE -D_FEATURES_H   

PROCPLSFLAGS=sqlcheck=full code=cpp DYNAMIC=ANSI USERID=dbcustadm/test123@ngcustdb SQLCHECK=SEMANTICS LINE=true
PROCINCL = include=$(WORKDIR)/include include=$(TUXINCL) include=$(ORAINCL1) include=$(ORAINCL2) include=$(ORAINCL3) include=/usr/include include=/usr/include/sys include=/opt/aCC include=/opt/aCC/include include=/opt/aCC/include/iostream
PRECOMP=$(ORADIR)/bin/proc

all: libBossSys.a 

OBJ1=CLBase.o CLRecordSet.o CLCommand.o db.o CLDbDriver.o CLDbug.o CLDriver.o CLError.o CLException.o CLFile.o CLLayer.o CLMalloc.o CLRule.o CLShmDriver.o CLString.o CLRight.o CLDomain.o unittype.o

libBossSys.a:$(OBJ1)
	ar -rv $@ $?
	mv $@ $(LIBDIR)

######################
.pc.cpp:
	$(PRECOMP) $(PROCPLSFLAGS) $(PROCINCL) iname=$< oname=$(<:.pc=.cpp)

.cpp.o:
	$(CC) $(INCLUDE) $(CFLAGS)  -c $*.cpp
clean:
	rm -f $(BINDIR)/core *.o *.lis
