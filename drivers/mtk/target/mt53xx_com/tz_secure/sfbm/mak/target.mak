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
ifndef RLS_CUSTOM_BUILD
RLS_CUSTOM_BUILD = false
endif

LINUX_ROOT ?= $(word 1, $(subst /vm_linux/,/vm_linux /, $(shell pwd)))
export PRJ_ROOT = $(LINUX_ROOT)/project_x/target/mt53xx_com/tz_secure/sfbm

ifndef CUSTOMER_ARCH_BUILD
DRV_PATH = $(PRJ_ROOT)
endif
ifndef KERNEL_VER
	KERNEL_VER := linux-3.0
endif

LINUX_PATH = $(LINUX_ROOT)/chiling/kernel/$(KERNEL_VER)

#export LINUX_DRV_ROOT = $(DRV_PATH)/mt5399
export DRV_ROOT = $(DRV_PATH)/mt5399
 #$(LINUX_DRV_ROOT)

DRV_INC = -I$(DRV_ROOT)/d_inc                   \
          -I$(DRV_ROOT)/$(DRIVER_DIR)/drv_cust    \
          -I$(DRV_ROOT)/$(DRIVER_DIR)/drv_inc     \
          -I$(DRV_ROOT)/$(DRIVER_DIR)/inc         \
          -I$(DRV_ROOT)/$(DRIVER_DIR)/private_inc \
          -I$(DRV_ROOT)/linux/lib/mw/x_inc            \
          -I$(DRV_ROOT)/linux/lib/mw/inc        \
          -I$(DRV_ROOT)/linux/osai/inc                \
          -I$(DRV_ROOT)/linux/include                \
          -I$(LINUX_PATH)/include    \
          -I$(LINUX_PATH)/arch/arm/include    \
          -I$(PRJ_ROOT)/driver/target/d_inc       \
          -I$(INC_ROOT)/drv_inc 


# APP_DRV_TABLE_SHARE: start
ifndef VM_LINUX_ROOT
        VM_LINUX_ROOT := $(word 1, $(subst /vm_linux/,/vm_linux /, $(shell pwd)))
endif

SYS_BUILD_ROOT = $(VM_LINUX_ROOT)/project_x/sys_build/$(CUSTOMER)/$(MODEL_NAME)/vid
DRV_INC += -I$(SYS_BUILD_ROOT)
## APP_DRV_TABLE_SHARE: end

# APP_DRV_TABLE_SHARE
DRV_INC += -I$(SYS_BUILD_ROOT)

ifdef O
DRV_INC += -I$(O)/include/asm/arch/inc
endif

AUD_ROOT = $(DRV_ROOT)/$(DRIVER_DIR)/aud
AUD_INC = -I$(AUD_ROOT)/include             \
          -I$(AUD_ROOT)/adsp/include        \
          -I$(AUD_ROOT)/adsp/include_API    \
          -I$(AUD_ROOT)/audio \
          -I$(AUD_ROOT)/periph \
          -I$(AUD_ROOT)/adsp

TARGET_INC = -I.					                \
            -I$(DRV_ROOT)/$(DRIVER_DIR)/private_inc	\
            -I$(DRV_ROOT)/$(DRIVER_DIR)/drv_inc		\
            -I$(DRV_ROOT)/$(DRIVER_DIR)/inc 		    \
            -I$(DRV_ROOT)/linux/lib/mw/x_inc		        \
            -I$(DRV_ROOT)/linux/app_if/drvif                   \
            -I$(DRV_ROOT)/linux/cb

# MTAL_PORTING: start
ifndef VM_LINUX_ROOT
        VM_LINUX_ROOT := $(word 1, $(subst /vm_linux/,/vm_linux /, $(shell pwd)))
endif

TARGET_INC += -I$(VM_LINUX_ROOT)/project_x/middleware/res_mngr/drv
TARGET_INC += -I$(VM_LINUX_ROOT)/project_x/middleware/res_mngr
TARGET_INC += -I$(VM_LINUX_ROOT)/project_x/middleware
TARGET_INC += -I$(VM_LINUX_ROOT)/chiling/app_if/mtal/mtal_inc
TARGET_INC += -I$(VM_LINUX_ROOT)/project_x/target/mt53xx_com/mw_if2
# MTAL_PORTING: end

ifdef DRV_INC_DIR
TARGET_INC += -I$(DRV_INC_DIR)
endif

D_CUST_INC = -I$(DRV_ROOT)/../d_inc \
             -I$(DRV_ROOT)/d_inc

ifeq "$(OSAI_VER)" "2"
OSAI_INC = -I$(DRV_ROOT)/linux/osai_v2/inc
else
OSAI_INC = -I$(DRV_ROOT)/linux/osai/inc
endif

PSR_INC = -I$(DRV_ROOT)/$(DRIVER_DIR)/psr

PLAYMGR_ROOT = $(DRV_ROOT)/$(DRIVER_DIR)/playmgr_v2
PLAYMGR_INC = -I$(PLAYMGR_ROOT)

SWDMX_ROOT = $(DRV_ROOT)/$(DRIVER_DIR)/swdmx_v2
SWDMX_INC = -I$(SWDMX_ROOT)

DMX_ROOT = $(DRV_ROOT)/$(DRIVER_DIR)/dmx5399
DMX_INC = -I$(DMX_ROOT) -I$(DMX_ROOT)/include -I$(DMX_ROOT)/gcpu/iram -I$(DMX_ROOT)/cypher/aes -I$(DMX_ROOT)/drm/mtk_skb_driver/ -I$(DMX_ROOT)/drm/libtomcrypt-1.16/src/headers -I$(DMX_ROOT)/drm/libtommath-0.41 -I$(DMX_ROOT)/drm/mtk_hci/ -I$(DMX_ROOT)/drm/mtk_hci/nautilus_3.5_RC/ -I$(DMX_ROOT)/drm/mtk_hci/nautilus_3.5_RC/include -I$(DMX_ROOT)/drm/mtk_hci/nautilus_3.5_RC/include/local -I$(DMX_ROOT)/drm/mtk_hci/nautilus_3.5_RC/include/src/cmn/include

TZ_ROOT = $(VM_LINUX_ROOT)/project_x/target/mt53xx_com/tz_secure
TZ_INC = -I$(TZ_ROOT)/open/inc/skb -I$(TZ_ROOT)/open/inc/hci

VDEC_ROOT = $(DRV_ROOT)/$(DRIVER_DIR)/vdec_v2
VDEC_INC = -I$(VDEC_ROOT)             \
           -I$(VDEC_ROOT)/h264             \
           -I$(VDEC_ROOT)/mpeg4 \
           -I$(VDEC_ROOT)/mpv        \
           -I$(VDEC_ROOT)/wmv   \
           -I$(VDEC_ROOT)/rv   \
           -I$(VDEC_ROOT)/img \
           -I$(VDEC_ROOT)/avs \
           -I$(VDEC_ROOT)/vp6 \
           -I$(VDEC_ROOT)/vp8 \
           -I$(VDEC_ROOT)/raw

VENC_ROOT = $(DRV_ROOT)/$(DRIVER_DIR)/venc
VENC_INC = -I$(VENC_ROOT)          \
           -I$(VENC_ROOT)/h264
           
TUNER_ROOT = $(DRV_ROOT)/$(DRIVER_DIR)/brdcst_if/tuner_src
TUNER_INC = -I$(TUNER_ROOT)/Tuner \
            -I$(TUNER_ROOT)/TunerHAL \
            -I$(TUNER_ROOT)/CtrlBus \
            -I$(TUNER_ROOT)/TunerPool \
            -I$(TUNER_ROOT)/TunerNptv/system \
            -I$(TUNER_ROOT)/inc

ifeq "$(findstring 5399_MT5112EE,$(BRDCST_IF))" "5399_MT5112EE"
TUNER_INC += -I$(TUNER_ROOT)/TunerDTD
TUNER_INC += -I$(TUNER_ROOT)/TunerAnalogNA
DEFINES += -DCC_FEDTD
endif
ifeq "$(findstring 5399_MT5135,$(BRDCST_IF))" "5399_MT5135"
TUNER_INC += -I$(TUNER_ROOT)/TunerDVBT
TUNER_INC += -I$(TUNER_ROOT)/TunerDVBC
TUNER_INC += -I$(TUNER_ROOT)/TunerAnalogEU
DEFINES += -DCC_FEDVBT
DEFINES += -DCC_DVBT_SUPPORT
endif
ifeq "$(findstring 5399DUAL,$(BRDCST_IF))" "5399DUAL"
TUNER_INC += -I$(TUNER_ROOT)/TunerDTD
TUNER_INC += -I$(TUNER_ROOT)/TunerAnalogNA
TUNER_INC += -I$(TUNER_ROOT)/TunerDVBT
TUNER_INC += -I$(TUNER_ROOT)/TunerAnalogEU
DEFINES += -DCC_DVBT_SUPPORT
endif

ifeq "$(findstring 5399DVBTC,$(BRDCST_IF))" "5399DVBTC"
TUNER_INC += -I$(TUNER_ROOT)/TunerDVBT
TUNER_INC += -I$(TUNER_ROOT)/TunerDVBC
DEFINES += -DCC_DVBT_SUPPORT
endif

ifeq "$(findstring TC90517,$(BRDCST_IF))" "TC90517"
TUNER_INC += -I$(TUNER_ROOT)/Tuner3rdParty/TC90517
endif

ifeq "$(findstring MN88471,$(BRDCST_IF))" "MN88471"
TUNER_INC += -I$(TUNER_ROOT)/Tuner3rdParty/MN88471/
TUNER_INC += -I$(TUNER_ROOT)/TunerDVBT
endif

ifeq "$(findstring ATBM8848,$(BRDCST_IF))" "ATBM8848"
TUNER_INC += -I$(TUNER_ROOT)/Tuner3rdParty/ATBM8848
endif

ifeq "$(findstring ATBM8846E,$(BRDCST_IF))" "ATBM8846E"
TUNER_INC += -I$(TUNER_ROOT)/Tuner3rdParty/ATBM8846E
endif

ifeq "$(findstring CXD2820,$(BRDCST_IF))" "CXD2820"
TUNER_INC += -I$(TUNER_ROOT)/Tuner3rdParty/CXD2820
TUNER_INC += -I$(TUNER_ROOT)/TunerDVBT
endif
ifeq "$(findstring 8G80,$(BRDCST_IF))" "8G80"
TUNER_INC += -I$(TUNER_ROOT)/Tuner3rdParty/8G80
endif
TUNER_INC += -I$(TUNER_ROOT)/TunerATD/eu
TUNER_INC += -I$(TUNER_ROOT)/TunerATD/common
TUNER_INC += -I$(TUNER_ROOT)/TunerATD/na
NPTV_ROOT = $(DRV_ROOT)/$(DRIVER_DIR)/nptv5399
NPTV_INC += -I$(NPTV_ROOT)/inc \
		  -I$(NPTV_ROOT)/inc/hw \
		  -I$(NPTV_ROOT)/inc/drv \
		  -I$(NPTV_ROOT)/inc/sys \
		  -I$(NPTV_ROOT)/drv/tuner/system \
		  -I$(NPTV_ROOT)/inc/empty \
		  -I$(NPTV_ROOT)/inc/tmp

VDP_ROOT = $(DRV_ROOT)/$(DRIVER_DIR)/vdp5399

VDP_INC += -I$(VDP_ROOT)

OSD_INC += -I$(DRV_ROOT)/$(DRIVER_DIR)/osd6896

export DRIVER_INC = $(DRV_INC) $(TARGET_INC) $(OSD_INC) $(AUD_INC) $(VDEC_INC) $(VENC_INC) $(PLAYMGR_INC) $(SWDMX_INC) $(D_CUST_INC) $(PSR_INC) $(OSAI_INC) $(TUNER_INC) $(NPTV_INC) $(VDP_INC) $(DMX_INC) $(TZ_INC)

DEFINES += -D_CPU_LITTLE_ENDIAN_  $(EXTRA_DEFINES) \
                   -DI1394_DRV_ONLY                         \
                   -DCC_SCPOS_EN                            \
                   -DCC_SRM_ON                              

# below definitions are for host runing fbm to detect secure FBM size  
DEFINES += -DCC_MEM_TOOL          \
	   -DCC_CHA_CHB_NO_GAP    \
	   -DCC_MT5395 
# above is for getting rid of FBM_GetCHBStartAddr()... maybe bad idea.
# for host running fbm as arm platform kernel space....
#DEFINES += -D__KERNEL__
DEFINES += -DPAGE_SIZE=0x1000
# end

DEFINES += -DCC_MT5399
DEFINES += -DCC_53XX_SWDMX_V2
ifneq "$(LINUX_SOLUTION)" "true"
DEFINES += -DCC_DRIVER_PROGRAM
DEFINES += -DDEFAULT_PANEL_SELECT=0

ifeq "$(KEY_FROM_DFB)" "y"
DEFINES += -DKEY_FROM_DFB
endif

endif
DEFINES += -Wall -Wno-format 

DEFINES += -DMT53XX_OSAI_MODULE -DMT53XX_FB_MODULE
DEFINES += -DCC_DYNAMIC_POWER_ONOFF

ifeq "$(BUILD_CFG)" "debug"
DEFINES += -g
else
DEFINES += -DNDEBUG
endif

OPTIMIZE_LVL ?=
ifneq "$(OPTIMIZE_LVL)" "-O2"
DEFINES += $(OPTIMIZE_LVL)
endif

ifndef WARNING_TO_ERROR
export WARNING_TO_ERROR =true
endif

ifneq "$(shell echo $(WARNING_TO_ERROR) | tr '[:lower:]' '[:upper:]')" "FALSE"
DEFINES += -Werror
endif
DEFINES += -pipe
export ASM_DEFINES = -Wa,--fatal-warnings

export DEFINES			:= $(sort $(DEFINES))

DEFINES += -Wframe-larger-than=4096
#
# Get Host specfic definitions
#
PROJECT_ROOT:=$(word 1, $(subst /project_x/,/project_x /,$(shell pwd)))
include $(PROJECT_ROOT)/host.mak

