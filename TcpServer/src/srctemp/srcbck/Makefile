PUBDIR=/bossver/appver/iccsvr/public
TUXDIR=/bea/tuxedo10/tuxedo10gR3
LIBDIR=$(TUXDIR)/lib
XCBINDIR=$(TUXDIR)/bin
BINDIR=$(PUBDIR)/bin

.SUFFIXES:.cpp .c .o
TUXINCL = ${TUXDIR}/include
CFLAGS= -I$(TUXINCL) -I$(PUBDIR)/include

CC=/bin/cc +DD64 

all:OTATcpServer 

COMPILE= $(XCBINDIR)/buildclient -w -f "$(OBJ_PUBLIC)" -l libencrypt.a
OBJ_PUBLIC=TcpServer.o PubFunc.o SysVar.o 

######################################
OTATcpServer: $(OBJ_PUBLIC)
	rm -f OTATcpServer
	$(COMPILE) -o OTATcpServer 
	cp OTATcpServer /inter/bin/.

######################################
.c.o:
	$(CC) $(CFLAGS) -c -g $<
clean:
	rm -f *.o

