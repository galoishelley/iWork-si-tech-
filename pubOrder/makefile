# Created: Tophill(zhangkun)
# Date:    2009.03.23

#!/bin/sh
.SUFFIXES:.cp

BINDIR=/iboss1/run/public/pubOrder/bin
PUBDIR=/iboss1/run/public/pubOrder

PUBINCL = $(PUBDIR)/include
PUBLIBS = $(PUBDIR)/lib -lPubOrder -lDesMac
TUXINCL = $(TUXDIR)/include
TUXLIBS = $(TUXDIR)/lib -ltux -lbuft -lfml -lfml32 -lwsc -lnsl

CFLAGS= -I. -I$(ORACLE_HOME)/precomp/public -I$(ORACLE_HOME)/rdbms/public -I$(ORACLE_HOME)/rdbms/demo -I$(ORACLE_HOME)/plsql/public -I$(ORACLE_HOME)/network/public -I$(PUBINCL) -I$(TUXINCL)

LFLAGS=-L$(ORACLE_HOME)/lib -lclntsh -L$(PUBLIBS) # -L$(TUXLIBS)

PROCFLAGS = include=$(ORACLE_HOME) include=${TUXINCL} include=${PUBINCL} DYNAMIC=ANSI USERID=dbcustadm/abc123@ngcustdb sqlcheck=SEMANTICS parse=FULL LINE=TRUE

PRECOMP=proc

#CC=cc +DA2.0W +DS2.0 -DSS_64BIT_SERVER -g
CC=aCC +DD64 -Ae -DPLATFORM_TYPE=HP -DMACHINE_WORD=64 +Z  -w -D__BIGMSGQUEUE_ENABLED -D__HP_NO_STRING_OVERLOADS -D__USE_XOPEN_EXTENDED -D_XOPEN_SOURCE_EXTENDED -D_INCLUDE_POSIX4_SOURCE -D_HPUX_SOURCE -D_FEATURES_H

##all:orderTransfer orderDeal orderStop clean
all:orderTransferCrm  orderDealCrm  orderReDeal orderStop clean

OBJ1=zktest.o
zktest:${OBJ1}
	$(CC) -o $(BINDIR)/$@ ${OBJ1} ${LFLAGS}

OBJ2=orderTransferCrm.o
orderTransferCrm:${OBJ2}
	$(CC) -o $(BINDIR)/$@ ${OBJ2} ${LFLAGS}

OBJ3=orderStop.o
orderStop:${OBJ3}
	$(CC) -o $(BINDIR)/$@ ${OBJ3} ${LFLAGS}

OBJ4=dncrypt.o
dncrypt:${OBJ4}
	$(CC) -o $(BINDIR)/$@ ${OBJ4} ${LFLAGS}

OBJ5=orderDealCrm.o
orderDealCrm:${OBJ5}
	$(CC) -o $(BINDIR)/$@ ${OBJ5} ${LFLAGS} -L$(TUXLIBS)

OBJ6=mytest.o
mytest:${OBJ6}
	$(CC) -o $(BINDIR)/$@ ${OBJ6} ${LFLAGS}

OBJ7=myfind.o
myfind:${OBJ7}
	$(CC) -o $(BINDIR)/$@ ${OBJ7} ${LFLAGS}

OBJ8=zktest1.o
zktest1:${OBJ8}
	$(CC) -o $(BINDIR)/$@ ${OBJ8} ${LFLAGS}

OBJ9=orderTransferBoss.o
orderTransferBoss:${OBJ9}
	$(CC) -o $(BINDIR)/$@ ${OBJ9} ${LFLAGS}

OBJ10=orderDealBoss.o
orderDealBoss:${OBJ10}
	$(CC) -o $(BINDIR)/$@ ${OBJ10} ${LFLAGS} -L$(TUXLIBS)

OBJ11=singleOrder.o
singleOrder:${OBJ11}
	$(CC) -o $(BINDIR)/$@ ${OBJ11} ${LFLAGS} -L$(TUXLIBS)

OBJ12=singleOpAccept.o
singleOpAccept:${OBJ12}
	$(CC) -o $(BINDIR)/$@ ${OBJ12} ${LFLAGS} -L$(TUXLIBS)

OBJ13=singleDealBoss.o
singleDealBoss:${OBJ13}
	$(CC) -o $(BINDIR)/$@ ${OBJ13} ${LFLAGS} -L$(TUXLIBS)

OBJ14=orderReDeal.o
orderReDeal:${OBJ14}
	$(CC) -o $(BINDIR)/$@ ${OBJ14} ${LFLAGS} -L$(TUXLIBS)

.cp.c:
	$(PRECOMP) $(PROCFLAGS) $<
		
.cp.o:
	$(PRECOMP) $(PROCFLAGS) $<
	$(CC) $(CFLAGS) -c $(<:.cp=.c)
.c.o:
	$(CC) $(CFLAGS) -c $<	
clean:
	rm -f *.o *.c *.lis
