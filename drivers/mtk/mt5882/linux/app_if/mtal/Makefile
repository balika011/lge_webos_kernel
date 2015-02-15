###############################################################################
# Copyright Statement:                                                        #
#                                                                             #
#   This software/firmware and related documentation ("MediaTek Software")    #
# are protected under international and related jurisdictions'copyright laws  #
# as unpublished works. The information contained herein is confidential and  #
# proprietary to MediaTek Inc. Without the prior written permission of        #
# MediaTek Inc., any reproduction, modification, use or disclosure of         #
# MediaTek Software, and information contained herein, in whole or in part,   #
# shall be strictly prohibited.                                               #
# MediaTek Inc. Copyright (C) 2010. All rights reserved.                      #
#                                                                             #
#   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND      #
# AGREES TO THE FOLLOWING:                                                    #
#                                                                             #
#   1)Any and all intellectual property rights (including without             #
# limitation, patent, copyright, and trade secrets) in and to this            #
# Software/firmware and related documentation ("MediaTek Software") shall     #
# remain the exclusive property of MediaTek Inc. Any and all intellectual     #
# property rights (including without limitation, patent, copyright, and       #
# trade secrets) in and to any modifications and derivatives to MediaTek      #
# Software, whoever made, shall also remain the exclusive property of         #
# MediaTek Inc.  Nothing herein shall be construed as any transfer of any     #
# title to any intellectual property right in MediaTek Software to Receiver.  #
#                                                                             #
#   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its  #
# representatives is provided to Receiver on an "AS IS" basis only.           #
# MediaTek Inc. expressly disclaims all warranties, expressed or implied,     #
# including but not limited to any implied warranties of merchantability,     #
# non-infringement and fitness for a particular purpose and any warranties    #
# arising out of course of performance, course of dealing or usage of trade.  #
# MediaTek Inc. does not provide any warranty whatsoever with respect to the  #
# software of any third party which may be used by, incorporated in, or       #
# supplied with the MediaTek Software, and Receiver agrees to look only to    #
# such third parties for any warranty claim relating thereto.  Receiver       #
# expressly acknowledges that it is Receiver's sole responsibility to obtain  #
# from any third party all proper licenses contained in or delivered with     #
# MediaTek Software.  MediaTek is not responsible for any MediaTek Software   #
# releases made to Receiver's specifications or to conform to a particular    #
# standard or open forum.                                                     #
#                                                                             #
#   3)Receiver further acknowledge that Receiver may, either presently        #
# and/or in the future, instruct MediaTek Inc. to assist it in the            #
# development and the implementation, in accordance with Receiver's designs,  #
# of certain softwares relating to Receiver's product(s) (the "Services").    #
# Except as may be otherwise agreed to in writing, no warranties of any       #
# kind, whether express or implied, are given by MediaTek Inc. with respect   #
# to the Services provided, and the Services are provided on an "AS IS"       #
# basis. Receiver further acknowledges that the Services may contain errors   #
# that testing is important and it is solely responsible for fully testing    #
# the Services and/or derivatives thereof before they are used, sublicensed   #
# or distributed. Should there be any third party action brought against      #
# MediaTek Inc. arising out of or relating to the Services, Receiver agree    #
# to fully indemnify and hold MediaTek Inc. harmless.  If the parties         #
# mutually agree to enter into or continue a business relationship or other   #
# arrangement, the terms and conditions set forth herein shall remain         #
# effective and, unless explicitly stated otherwise, shall prevail in the     #
# event of a conflict in the terms in any agreements entered into between     #
# the parties.                                                                #
#                                                                             #
#   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and     #
# cumulative liability with respect to MediaTek Software released hereunder   #
# will be, at MediaTek Inc.'s sole discretion, to replace or revise the       #
# MediaTek Software at issue.                                                 #
#                                                                             #
#   5)The transaction contemplated hereunder shall be construed in            #
# accordance with the laws of Singapore, excluding its conflict of laws       #
# principles.  Any disputes, controversies or claims arising thereof and      #
# related thereto shall be settled via arbitration in Singapore, under the    #
# then current rules of the International Chamber of Commerce (ICC).  The     #
# arbitration shall be conducted in English. The awards of the arbitration    #
# shall be final and binding upon both parties and shall be entered and       #
# enforceable in any court of competent jurisdiction.                         #
###############################################################################
###########################################################################
# $RCSfile: Makefile,v $
# $Revision: #3 $
# $Date: 2015/01/02 $
# $Author: billy.park $
#
# Description:
#         Leave-level makefile to build the subcomponent of driver library.
#
#         Specify the source files to be compile in SRC.
#############################################################################

THIS_COMPONENT = mtal


ifndef DRV_ROOT
export DRV_ROOT = ../../../..
endif

SRC = io_mtadac.c    io_mtosd.c         io_mtscart.c  io_mtvdecex.c \
      io_mtauddec.c  io_mtgfx.c      io_mtperipheral.c  io_mtvbiex.c  io_mtvdo.c \
      io_mtdmx.c     io_mtpmx.c         io_mtvdec.c  \
      io_mtvfe.c     mtal_if.c       mt_audio.c         io_mttve.c \
      io_mttsrec.c   io_mtdrec.c           io_mtb2r.c io_mtstc.c mt_aud_mw.c\
      io_mtfeeder.c  io_mtmx.c io_mtmhl.c\
      io_mtvss.c    io_mtdrm.c io_mtdrm_wv.c io_mtfe_tunerhal.c    io_mtsys.c  drv_cust_api.c io_mtdmx_mwif.c \
      io_mtfe_tuner.c
#Disable pvr/dvr function in A5LR Project
#io_mtvenc.c

SRC+= io_mtrm.c io_mtdrvcust.c
SRC+= io_mtphoto4k.c

ifdef ENABLE_MULTIMEDIA
SRC+= io_mtswdmx.c    io_mtpvr.c    io_mtmplayer.c    mtal_mtmplayer_slide.c io_mtimage.c
endif

ifneq "$(DISCARD_SUPPORT_PCMCIA)" "true"
SRC+= io_mtci.c
endif


SRC+= io_mtcec.c


ifeq "$(DISCARD_SUPPORT_PCMCIA)" "true"
DEFINES += -DCC_DISCARD_SUPPORT_PCMCIA
endif

##US
ifeq "$(findstring DTD,$(BRDCST_IF))" "DTD"
SRC+= io_mtfe_atsc.c
DEFINES += -DCC_FEDTD
endif
##EU 
ifeq "$(findstring DVBT,$(BRDCST_IF))" "DVBT"
SRC+= io_mtfe_dvbt.c
endif

ifeq "$(findstring DVBC,$(BRDCST_IF))" "DVBC"
SRC+= io_mtfe_dvbc.c
DEFINES += -DCC_FEDVBC 
endif

ifeq "$(findstring DVBTC,$(BRDCST_IF))" "DVBTC"
SRC+= io_mtfe_dvbt.c io_mtfe_dvbc.c
DEFINES += -DCC_FEDVBC
DEFINES += -DCC_FEDVBT 
# For DVBTC Lei Li 20130228
#CC_INC += -I$(VM_LINUX_ROOT)/project_x/target/mt53xx_com/53xx_com_driver/brdcst_if/tuner_src/TunerDVBT
#CC_INC += -I$(VM_LINUX_ROOT)/project_x/target/mt53xx_com/53xx_com_driver/brdcst_if/tuner_src/TunerDVBC
endif

ifeq "$(findstring VDRDemodLST,$(BRDCST_IF))" "VDRDemodLST"
SRC+= io_mtfe_dvbt.c io_mtfe_dvbc.c
DEFINES += -DCC_FEDVBC
DEFINES += -DCC_FEDVBT 
endif

##JAPAN
ifeq "$(findstring TC90522,$(BRDCST_IF))" "TC90522"
SRC+= io_mtfe_isdbt.c io_mtfe_isdbs.c
DEFINES += -DCC_FEISDB
endif

##BRAZIL
ifeq "$(findstring TC90517,$(BRDCST_IF))" "TC90517"
SRC+= io_mtfe_sbtvd.c
DEFINES += -DCC_FESBTVD
endif

#ifeq "$(findstring VDRISDBT,$(BRDCST_IF))" "VDRISDBT"
#SRC+= io_mtfe_sbtvd.c
#DEFINES += -DCC_FESBTVD
#endif

ifeq "$(findstring TC90527,$(CUST_DEMOD))" "TC90527"
SRC+= io_mtfe_sbtvd.c
DEFINES += -DCC_FESBTVD
endif


##EU 
ifeq "$(findstring 5135,$(BRDCST_IF))" "5135"
SRC+= io_mtfe_dvbt.c io_mtfe_dvbc.c
DEFINES += -DCC_FEDVBT
DEFINES += -DCC_FEDVBC
endif

##CH
ifeq "$(findstring 8G80,$(BRDCST_IF))" "8G80"
SRC+= io_mtfe_dtmb.c
DEFINES += -DCC_FEDTMB
endif

ifeq "$(findstring 5175,$(BRDCST_IF))" "5175"
SRC+= io_mtfe_dtmb.c
DEFINES += -DCC_FEDTMB
DEFINES += -DCC_MT5175DTMB
endif

ifeq "$(findstring DTMBC,$(BRDCST_IF))" "DTMBC"
SRC+= io_mtfe_dtmb.c io_mtfe_dvbc.c
DEFINES += -DCC_FEDTMB
DEFINES += -DCC_FEDVBC
DEFINES += -DCC_SOCDTMB  
endif


##ATSC
ifeq "$(findstring MT5112EE,$(BRDCST_IF))" "MT5112EE"
SRC+= io_mtfe_atsc.c
DEFINES += -DCC_FEDTD
endif

ifeq "$(ENABLE_ADC_SIF_BYPASS)" "true"
SRC+= io_mtfe_pal.c 
DEFINES += -DENABLE_SIF_BYPASS
else
#dummy
ifeq "$(findstring atd,$(BRDCST_IF))" "atd"
SRC+= io_mtfe_pal.c io_mtfe_ntsc.c io_mtfe_atd.c
DEFINES += -DUSE_ATD_IF_DEMOD
DEFINES += -DCC_FEATD
endif
endif

#TZ
ifeq "$(TRUSTZONE)" "true"
SRC+= io_mttz.c
endif

#HDCP2
ifeq "$(ENABLE_HDCP2)" "true"
    ifeq "$(TRUSTZONE)" "true"
        DEFINES += -DCC_ENABLE_HDCP2
    endif
endif

OBJ =

SUB_COMPONENTS +=	hash \
			time_msrt

OPTIONAL_SUB_COMPONENTS =

DEFINES +=

CC_INC += -I$(DRV_ROOT)/linux/app_if/mtal/include
CC_INC += -I$(DRV_ROOT)/linux/include/linux
CC_INC += -I$(DRV_ROOT)/linux/native/mt53xx_fb
CC_INC += -I$(DRV_ROOT)/linux/ioctl/vomx


     
# MTAL_PORTING: start
ifndef VM_LINUX_ROOT
        VM_LINUX_ROOT := $(word 1, $(subst /vm_linux/,/vm_linux /, $(shell pwd)))
endif

CC_INC += -I$(VM_LINUX_ROOT)/project_x/middleware/res_mngr
CC_INC += -I$(VM_LINUX_ROOT)/project_x/middleware
CC_INC += -I$(VM_LINUX_ROOT)/chiling/app_if/mtal/mtal_inc
CC_INC += -I$(VM_LINUX_ROOT)/project_x/target/mt53xx_com/mw_if2
# MTAL_PORTING: end

########## for drm_hdcp2.c Beginning##########
ifeq "$(TARGET_IC)" "mt5398"
CC_INC += -I$(DRV_ROOT)/target/mt53xx_com/53xx_com_driver/dmx9880/drm/mtk_skb_driver
else
CC_INC += -I$(DRV_ROOT)/target/mt53xx_com/53xx_com_driver/dmx6896/drm
endif

########## for color covert init##########
ifdef VDEC_YUV_CVT_IMG
CC_INC += -I$(DRV_ROOT)/target/mt53xx_com/53xx_com_driver/vdec_v2
endif

DEFINES += -DLTC_SOURCE -DLTC_NO_FILE
ifeq "$(TOM_MATH)" "fast"
DEFINES += -DTFM_DESC
else
DEFINES += -DLTM_DESC
endif

ifeq "$(TARGET_IC)" "mt5398"
CC_INC += -I$(DRV_ROOT)/target/mt53xx_com/53xx_com_driver/dmx9880/drm/libtomcrypt-1.16/src/headers
else
CC_INC += -I$(DRV_ROOT)/target/mt53xx_com/53xx_com_driver/dmx6896/drm/libtomcrypt-1.16/src/headers
endif

ifeq "$(TOM_MATH)" "fast"
ifeq "$(TARGET_IC)" "mt5398"
CC_INC += -I$(DRV_ROOT)/target/mt53xx_com/53xx_com_driver/dmx9880/drm/tomsfastmath-0.14/src/headers
else
CC_INC += -I$(DRV_ROOT)/target/mt53xx_com/53xx_com_driver/dmx6896/drm/tomsfastmath-0.14/src/headers
endif
else
ifeq "$(TARGET_IC)" "mt5398"
CC_INC += -I$(DRV_ROOT)/target/mt53xx_com/53xx_com_driver/dmx9880/drm/libtommath-0.41
else
CC_INC += -I$(DRV_ROOT)/target/mt53xx_com/53xx_com_driver/dmx6896/drm/libtommath-0.41
endif
endif
########## for drm_hdcp2.c End##########

EXTRA_CFLAGS += -O0

#############################################################################
#
# Include the makefile common to all components
#
ifneq "$(TV_DRV_VFY)" "true"
include $(DRV_ROOT)/driver.mak
endif
