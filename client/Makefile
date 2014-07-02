PUBDIR=/iboss1/run/public
TUXDIR=/bea/tuxedo8.1
LIBDIR=$(TUXDIR)/lib
XCBINDIR=$(TUXDIR)/bin
BINDIR=$(PUBDIR)/bin

.SUFFIXES:.cpp .c .o
TUXINCL = ${TUXDIR}/include
CFLAGS= -I$(TUXINCL) -I$(PUBDIR)/include

CC=/bin/cc +DD64
# Sys librarys, used by multithread system
#STDCC_LIBS = /lib/crt0_r.o -lsvld -lc_r -lpthreads -lodm -lm


#
# Tuxedo 6.5 include directory and librarys 
#
#TUXLIBS = ${TUXDIR}/lib -ltux -ltux2 -lbuft -lfml -lfml32 -lgp -lnwi -lnws -lwsc 

# Path of target file, current path is set
all:s_public_InitShm s_public_DelShm s_public_GetShm s_public_GetPasswd s_public_GetMemAccept redo s_public_GetRedo cl32
# Invoke tuxedo compiler
COMPILE= $(XCBINDIR)/buildclient -f "$(OBJ_PUBLIC)" -l $(PUBDIR)/lib/libDesMac.a
OBJ_PUBLIC=s_public.o

######################################
s_public_InitShm: $(OBJ_PUBLIC) s_public_InitShm.o
	rm -f $(BINPATH)/s_public_InitShm
	$(COMPILE)  -f s_public_InitShm.o -o $(BINDIR)/s_public_InitShm

s_public_DelShm: $(OBJ_PUBLIC) s_public_DelShm.o
	rm -f $(BINPATH)/s_public_DelShm
	$(COMPILE)  -f s_public_DelShm.o -o $(BINDIR)/s_public_DelShm

s_public_GetShm: $(OBJ_PUBLIC) s_public_GetShm.o
	rm -f $(BINPATH)/s_public_GetShm
	$(COMPILE)  -f s_public_GetShm.o -o $(BINDIR)/s_public_GetShm

s_public_GetPasswd: $(OBJ_PUBLIC) s_public_GetPasswd.o
	rm -f $(BINPATH)/s_public_GetPasswd
	$(COMPILE)  -f s_public_GetPasswd.o -o $(BINDIR)/s_public_GetPasswd

s_public_GetMemAccept: $(OBJ_PUBLIC) s_public_GetMemAccept.o
	rm -f $(BINPATH)/s_public_GetMemAccept
	$(COMPILE)  -f s_public_GetMemAccept.o -o $(BINDIR)/s_public_GetMemAccept

redo: $(OBJ_PUBLIC) redo.o
	rm -f $(BINPATH)/redo
	$(COMPILE)  -f redo.o -o $(BINDIR)/redo

s_public_GetRedo: $(OBJ_PUBLIC) s_public_GetRedo.o
	rm -f $(BINPATH)/s_public_GetRedo
	$(COMPILE)  -f s_public_GetRedo.o -o $(BINDIR)/s_public_GetRedo

cl32: $(OBJ_PUBLIC) cl32.o
	rm -f $(BINPATH)/cl32
	$(COMPILE)  -f cl32.o -o $(BINDIR)/cl32

cl32arr: $(OBJ_PUBLIC) cl32arr.o
	rm -f $(BINPATH)/cl32arr
	$(COMPILE)  -f cl32arr.o -o $(BINDIR)/cl32arr

######################################
.c.o:
	$(CC) $(CFLAGS) -c $<
clean:
	rm -f *.o

32wh

clc32: $(OBJ_PUBLIC) clc32.o
	rm -f $(BINPATH)/clc32
	$(COMPILE)  -f clc32.o -o $(BINDIR)/clc32

cl32test: $(OBJ_PUBLIC) cl32test.o
	rm -f $(BINPATH)/cl32test
	$(COMPILE)  -f cl32test.o -o $(BINDIR)/cl32test


######################################
.c.o:
	$(CC) $(CFLAGS) -c $<
clean:
	rm -f *.o

