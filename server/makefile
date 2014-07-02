#!/usr/bin/ksh

include $(HOME)/run/mkmng/cstdmk.hp
REMIND_MSG=请指定具体的编译目标make lib
DBPARAMETER=USERID=dbchange/dbu0i9o8-.@ngcustdb

lib: libbosspub.a
OBJ1= publicP.o publicT.o publicM.o publicS.o publicF.o publicDb.o \
	publicShm.o pubInclude.o pubServChk.o pubFunction.o \
	pubDsmpFunc.o netCardPubDb.o netCardPubFun.o public1861.o \
	publicBill.o pubSendPBOSSWs.o pubCancel.o \
	chnGroup.o pubChgFunc.o pubProduct.o funcParam.o \
	pubInnet.o markPublic.o fundPublic.o pubAdcInterFace.o pubProdCtrl.o \
	publicGrpCancel.o publicGrp.o RPubGrp.o include13.o \
	pubBossOrder.o pubDsmpOrder.o pubDataOrder.o pubQryCall.o publicNG.o \
	pubBusiOrderCallCRM.o pubBusiOrderDealBOSS.o pubBusiOrderCallBOSS.o pubBusiOrderDealCRM.o \
	pubBusiOrderCallEM.o publicEmer.o emPublicFunc.o \
	pub_smap.o HttpConnet.o \
	nmslog.o publicLog.o tux_server_dummy.o \
    pubPbossInterFace.o pubTicketSnd.o


libbosspub.a:$(OBJ1)
	ar -rv libbosspub.a $(OBJ1)
	mv -f $@ $(PUBDIR)/lib
	@echo "### $@ loaded ###\n"


