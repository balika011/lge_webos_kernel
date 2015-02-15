##############################################################################
# No Warranty                                                                #
# Except as may be otherwise agreed to in writing, no warranties of any      #
# kind, whether express or implied, are given by MTK with respect to any MTK #
# Deliverables or any use thereof, and MTK Deliverables are provided on an   #
# "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        #
# including any implied warranties of merchantability, non-infringement and  #
# fitness for a particular purpose and any warranties arising out of course  #
# of performance, course of dealing or usage of trade.  Parties further      #
# acknowledge that Company may, either presently and/or in the future,       #
# instruct MTK to assist it in the development and the implementation, in    #
# accordance with Company's designs, of certain softwares relating to        #
# Company's product(s) (the "Services").  Except as may be otherwise agreed  #
# to in writing, no warranties of any kind, whether express or implied, are  #
# given by MTK with respect to the Services provided, and the Services are   #
# provided on an "AS IS" basis.  Company further acknowledges that the       #
# Services may contain errors, that testing is important and Company is      #
# solely responsible for fully testing the Services and/or derivatives       #
# thereof before they are used, sublicensed or distributed.  Should there be #
# any third party action brought against MTK, arising out of or relating to  #
# the Services, Company agree to fully indemnify and hold MTK harmless.      #
# If the parties mutually agree to enter into or continue a business         #
# relationship or other arrangement, the terms and conditions set forth      #
# hereunder shall remain effective and, unless explicitly stated otherwise,  #
# shall prevail in the event of a conflict in the terms in any agreements    #
# entered into between the parties.                                          #
##############################################################################
###########################################################################
#  
#  Copyright (c) 2007, MediaTek Inc.
#  All rights reserved 
#
#  Unauthorized use, practice, perform, copy, distribution, reproduction,
#  or disclosure of this information in whole or in part is prohibited.
###########################################################################
# $RCSfile: Makefile.tool,v $
# $Revision: #1 $
# $Date: 2014/12/25 $
# $Author: dtvbm11 $
#
# Description:
#         Leave-level makefile to build the subcomponent of Middleware library.
#
#         Specify the source files to be compile in SRC.
#############################################################################

CPP  = g++
CC   = gcc -m32
STRIP= strip

OBJ  = main.o dlm_api.o dlm_fm.o dlm_digest.o md5_slim.o dlm_aes.o efuse.o dlm_util.o dlm_gentool.o bigd.o bigdigits.o bigdRand.o spBigdigits.o spExtras.o spRandom.o
LINKOBJ  = main.o dlm_api.o dlm_fm.o dlm_digest.o md5_slim.o dlm_aes.o efuse.o dlm_util.o dlm_gentool.o bigd.o bigdigits.o bigdRand.o spBigdigits.o spExtras.o spRandom.o

LIBS = 
INCS =  -I../inc -I../os/inc -I../dbg -I../../middleware
CXXINCS = 

BIN  = codefile_v2

CXXFLAGS = $(CXXINCS) 
CFLAGS = $(INCS) -g  -Wall -DNO_MIDDLEWARE -D_CPU_LITTLE_ENDIAN_=1 -DDLM_USE_AES_MODE -DCC_DLM_V2 -DDLM_SHA_256 -DDLM_GCPU_SHA_256 -DDLM_GENERATE_TOOL
#-D_WIN_32 for windows
CFLAGS+= -I../../../oss/library/gnuarm-4.5.1/openssl/1.0.0/include

LDFLAGS += -L. -lcrypto

RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before codefile_v2 all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)*

$(BIN): $(OBJ)
	$(CC) -static $(LINKOBJ) -o "codefile_v2" $(LIBS) $(LDFLAGS)
	$(STRIP) --strip-unneeded "codefile_v2"

main.o: main.c
	$(CC) -c main.c -o main.o $(CFLAGS)

dlm_digest.o: dlm_digest.c
	$(CC) -c dlm_digest.c -o dlm_digest.o $(CFLAGS)
	
dlm_aes.o: dlm_aes.c
	$(CC) -c dlm_aes.c -o dlm_aes.o $(CFLAGS)
	
dlm_api.o: dlm_api.c
	$(CC) -c dlm_api.c -o dlm_api.o $(CFLAGS)

dlm_fm.o: dlm_fm.c
	$(CC) -c dlm_fm.c -o dlm_fm.o $(CFLAGS)

md5_slim.o: md5_slim.c
	$(CC) -c md5_slim.c -o md5_slim.o $(CFLAGS)

sha256_slim.o: sha256_slim.c
	$(CC) -c sha256_slim.c -o sha256_slim.o $(CFLAGS)

efuse.o: efuse.c
	$(CC) -c efuse.c -o efuse.o $(CFLAGS)

dlm_util.o: dlm_util.c                       	
	$(CC) -c dlm_util.c -o dlm_util.o $(CFLAGS)

dlm_gentool.o: dlm_gentool.c
	$(CC) -c dlm_gentool.c -o dlm_gentool.o $(CFLAGS)

bigd.o: bigd.c
	$(CC) -c bigd.c -o bigd.o $(CFLAGS)

bigdigits.o: bigdigits.c
	$(CC) -c bigdigits.c -o bigdigits.o $(CFLAGS)
	
bigdRand.o: bigdRand.c
	$(CC) -c bigdRand.c -o bigdRand.o $(CFLAGS)

spRandom.o: spRandom.c
	$(CC) -c spRandom.c -o spRandom.o $(CFLAGS)
	
spExtras.o: spExtras.c
	$(CC) -c spExtras.c -o spExtras.o $(CFLAGS)
	
spBigdigits.o: spBigdigits.c
	$(CC) -c spBigdigits.c -o spBigdigits.o $(CFLAGS)
