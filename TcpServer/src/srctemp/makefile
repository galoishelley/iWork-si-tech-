#!/bin/sh
.SUFFIXES:.cpp .c .o

WORKDIR=/inter/interapp
BINDIR=$(WORKDIR)/bin
PUBDIR=/inter/interapp/src/public

CFLAGS= -I$(PUBDIR)/include -I$(TUXDIR)/include 

CC=cc +DD64 

all:TcpServer 

OBJ_PUBLIC=TcpServer.o PubFunc.o SysVar.o
COMPILE=buildclient  -w -f " $(OBJ_PUBLIC) " -l libencrypt.a 

######################################
TcpServer: $(OBJ_PUBLIC)
	rm -f TcpServer
	$(COMPILE) -o $(BINDIR)/TcpServer 

######################################
.c.o:
	$(CC) $(CFLAGS) -c -g $<
clean:
	rm -f *.o

