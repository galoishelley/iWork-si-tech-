CFLAGS=+DD64 -DSS_64BIT_SERVER -I. -I$(ORACLE_HOME)/precomp/public -I$(ORACLE_HOME)/rdbms/public -I$(ORACLE_HOME)/rdbms/demo -I$(ORACLE_HOME)/plsql/public -I$(ORACLE_HOME)/network/public 

LFLAGS=-L$(ORACLE_HOME)/lib/ -lclntsh `cat $(ORACLE_HOME)/lib/ldflags`   `cat $(ORACLE_HOME)/lib/sysliblist`  -lm  -lpthread -L../lib -lacctcomm

PRECOMP=$(ORACLE_HOME)/bin/proc

.SUFFIXES: .pc .c .o

OBJ1=querybill_server.o daemon.o sigtool.o rtPayQry.o rtPayPub.o
OBJ2=querybill_client.o
OBJ3=querybill_test.o
OBJ4=PayBcpMain.o daemon.o sigtool.o rtPayQry.o rtPayPub.o rtPayBcp.o

all:libquerybill.a clean

libquerybill.a:$(OBJ2)
	ar -rv $@ $(OBJ2)
	mv libquerybill.a /iboss1/run/public/lib

.c.o:
	@echo " Compiling the $<"
	$(CC)  -c  $(CFLAGS) $(LFLAGS) $<

clean:
	rm -f *.o
	rm -f rtPayBcp.c PayBcpMain.c
	rm -f *.lis
