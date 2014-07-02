#!/usr/bin/ksh

include $(HOME)/run/mkmng/cstdmk.hp

REMIND_MSG=请指定具体的编译目标，例如:make lib
DBPARAMETER=USERID=dbchange/dbu0i9o8-.@ngcustdb
CUSTOMS_LIB=
CUSTOMS_HEAD=-I../include


lib:libShortMsg.a
	@echo libShortMsg.a has been updated!!

SHORTMSGOBJS=pubShortMsg.o

libShortMsg.a:$(SHORTMSGOBJS)
	ar cr $@ $(SHORTMSGOBJS)
##	mv $@ ../../lib

