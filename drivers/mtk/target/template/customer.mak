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
# $RCSfile: customer.mak,v $
# $Revision: #1 $
# $Date: 2015/02/15 $
# $Author: p4admin $
# $MD5HEX: 61118bc277d8ac078b83c14a0d509c1a $
#
# Description:
#         Makefile to build a "MT-5351" demo board build. The following
#         targets are supported:
#
#             all:           Compiles middleware, custom, target and this
#                            directory and creates a final image.
#             mw:            Compiles the middleware.
#             x_inc:         Creates and copies the middleware header files.
#             c_inc:         Creates and copies the application header files.
#             custom:        Compiles the custom.
#             target:        Compiles the MT-5351 target.
#             image:         Compiles this directory and creates a final
#                            image.
#             loader:
#             packing:
#             clean:         Cleans the middleware, custom, target and this
#                            directory and removes the run image.
#             clean_mw:      Cleans the middleware.
#             clean_custom:  Cleans the custom.
#             clean_target:  Cleans the MT-5351 target.
#             clean_image:   Cleans this directory and removes the run image.
#             clean_force:   Forcefully removes all libray and object
#                            directories as well as the run image. A fast way
#                            to perform cleanup.
#             clean_all:     Forcefully removes all version(rel/cli/debug) of libray and object
#                            directories as well as the run image. A fast way
#                            to perform cleanup.
#             clean_loader:
#
#         The following commands are supported:
#
#             BUILD_CFG=debug
#                 Builds the specified target with symbolic debug info and the
#                 define file "mt5351_demo.def" is read. The run image is named
#                 is named "mt5351_demo_dbg".If BUILD_CFG is not specified then
#                 a non-debug build is created, which does not contain any
#                 symbolic debug information.
#
#             BRDCST_IF=<name>
#                 Allows the specifying of the front-end. The name must must
#                 be the directory name of the front-end, which follows the
#                 mt5351 targets brdcst_if sub-directory. For example,
#                 name may be set to "gen2004" or "5111" etc. In case
#                 BRDCST_IF is not set, "5111philips" is assumed.
#############################################################################

ifeq "$(LOADER_VERSION)" ""
LOADER_VERSION := $(VERSION)
endif

export TARGET
export BUILD_NAME
export CUSTOMER
export CUSTOM
export MODEL_NAME
export VERSION
export LOADER_VERSION
export THIS_ROOT



#
# Set the configuration
#
ifeq "$(BUILD_CFG)" "debug"
CFG_DEF := $(MODEL_NAME)_dbg.cfg
else

ifeq "$(BUILD_CFG)" "cli"
CFG_DEF := $(MODEL_NAME)_cli.cfg
else

CFG_DEF := $(MODEL_NAME).cfg
endif
endif


#
# Set the optimization level
#
ifdef OPTIMIZE_LVL
export OPTIMIZE_LVL
endif


ifndef UBOOT_LOADER
export UBOOT_LOADER = FALSE
endif

ifndef LOADER_MAX_SIZE
export LOADER_MAX_SIZE = 131072
endif

DEFINES += -DLOADER_MAX_SIZE=$(LOADER_MAX_SIZE)

ifdef PQDATA_ADDRESS
DEFINES += -DUSER_PQDATA_ADDRESS=$(PQDATA_ADDRESS)
endif

#
# Get all the path values set
#
ifndef PROJECT_ROOT
PROJECT_ROOT := $(word 1, $(subst /project_x/,/project_x /, $(shell pwd)))
endif

include $(PROJECT_ROOT)/path.mak
include $(TARGET_ROOT)/target.mak


#
# List the sources which must be compiled	#####
#
# SRC := sys_start.c
# IC dependent XXX
ifndef TARGET_DRIVER
export TARGET_DRIVER := $(subst mt,,$(TARGET))_driver
endif

#
# Add local compile defines
#
ifeq "$(word 1, $(subst mt,mt , $(TARGET)))" "mt"
TARGET_SPEC_DRV_CUST_HDR_FILE = d_$(word 2, $(subst mt,mt , $(TARGET)))_cust.h
DEFINES += -DTARGET_SPEC_DRV_CUST_HDR_FILE=\"$(TARGET_SPEC_DRV_CUST_HDR_FILE)\"
endif

#
# Add local include directives		#####
#
# CC_INC += -I$(CUSTOM_ROOT)/amb -I$(CUSTOM_ROOT)/app_util -I$(MW_ROOT) -I$(CUSTOM_ROOT)/am


#
# Add local tuner driver setting.	#####
#
# ifndef BRDCST_IF
# BRDCST_IF := 5111philips
# endif

#
# Target sub component list. Note that front end selection is handled specifically.
# If the target drivers would not require compilation flags, there would not have
# to be an inclusion of an additional definition file, which depends on the front
# end driver type.
#
ifndef TARGET_SUB_COMP_LIST
TARGET_SUB_COMP_LIST := 
endif

TARGET_SUB_COMP_LIST += mw_if diag

ifeq "$(CA_IF)" "pod_dk"
TARGET_SUB_COMP_LIST += digitalkeystone
endif

ifeq "$(1394_IF)" "vividlogic"
TARGET_SUB_COMP_LIST += vividlogic
endif

ifeq "$(TARGET)" "simulator"
TARGET_SUB_COMP_LIST += brdcst_if/$(BRDCST_IF) dummy start bsp dbg dump os
endif

#
# Add additional selective fonts
#
CUST_SEL_FONTS := $(addprefix $(CUSTOM_BASE)/,$(SEL_FONTS))

#
# Have to do that because the 5351 tuner driver requires knowledge of the specific
# front end.
#
NIM := $(BRDCST_IF)
export BRDCST_IF
export NIM


#
# Executable name
#
EXE_NAME := $(MODEL_NAME)

ifeq "$(BUILD_CFG)" "debug"
EXE_NAME := $(EXE_NAME)_dbg

else
ifeq "$(BUILD_CFG)" "cli"
EXE_NAME := $(EXE_NAME)_cli
endif
endif

MAP_NAME := $(EXE_NAME)$(MAP_SUFFIX)

ALL_EXE_NAME := $(MODEL_NAME) $(MODEL_NAME)_cli $(MODEL_NAME)_dbg 
export ALL_EXE_NAME
export EXE_NAME
export MAP_NAME

include $(PROJECT_ROOT)/object_type.mak

#
# Specify some root path's
#
OBJ_DIR    := $(THIS_ROOT)/obj/$(CUSTOM_OUTPUT)/$(OBJECT_TYPE)
DEPEND_DIR := $(THIS_ROOT)/obj/$(CUSTOM_OUTPUT)/$(OBJECT_TYPE)
BIN_DIR    := $(THIS_ROOT)/bin/$(CUSTOM_OUTPUT)/$(OBJECT_TYPE)
OBJ_ROOT_DIR    := $(THIS_ROOT)/obj
DEPEND_ROOT_DIR := $(THIS_ROOT)/obj
BIN_ROOT_DIR    := $(THIS_ROOT)/bin

export OBJ_DIR
export DEPEND_DIR
export BIN_DIR
export OBJ_ROOT_DIR
export DEPEND_ROOT_DIR
export BIN_ROOT_DIR

#
# Specify the object files with full path
#
OBJ      = $(SRC:.c=.o)
ABS_OBJ := $(addprefix $(OBJ_DIR)/, $(OBJ))

export OBJ
export ABS_OBJ

#
# Specify the dependability files
#
DEPEND     := $(SRC:.c=.d)
ABS_DEPEND := $(addprefix $(DEPEND_DIR)/, $(DEPEND))

export DEPEND
export ABS_DEPEND

#
# Specify the absolute executable and map file
#
ABS_EXE := $(BIN_DIR)/$(EXE_NAME)$(TARGET_EXE_SUFFIX)
ABS_MAP := $(BIN_DIR)/$(MAP_NAME)
LOADER_EXE := $(BIN_DIR)/loader/$(EXE_NAME)$(TARGET_EXE_SUFFIX)

export ABS_EXE
export ABS_MAP


#
# MT5351 specific commands
#
GZIP := gzip

ifndef MTKTOOL_BIN
MTKTOOL_BIN := $(EXE_NAME)_image.bin
endif

ALL_MTKTOOL_BIN := $(foreach i,$(ALL_EXE_NAME),$(i)_image.bin)
#
# DOS platform don't do packing.
#
ifeq "$(PROJECT_X_HOST)" "dos"
DOLOADER := loader
else
DOLOADER := loader packing
endif

#
# Packing tool
#
PACK_TOOL_DIR = $(TOOL_ROOT)/$(PACK_TOOL_NAME)
PACK_TOOL_NAME = mt53xx_pack
MT53XX_PACK = $(PACK_TOOL_DIR)/$(PACK_TOOL_NAME)_$(PROJECT_X_HOST)
MT53XX_PACKIMG = $(PACK_TOOL_DIR)/packimg
ifdef SERIAL_NUMBER
MT53XX_CMDLINE = $(MT53XX_PACK) $(EXE_NAME)_loader.bin $(EXE_NAME).bin.lzhs $(MTKTOOL_BIN) 2 "$(CUSTOMER)" "$(MODEL_NAME)" "$(SERIAL_NUMBER)" $(LOADER_MAX_SIZE) upg_ver
else
MT53XX_CMDLINE = $(MT53XX_PACK) $(EXE_NAME)_loader.bin $(EXE_NAME).bin.lzhs $(MTKTOOL_BIN) 2 "$(CUSTOMER)" "$(MODEL_NAME)" "$(LOADER_VERSION)" $(LOADER_MAX_SIZE) upg_ver
endif
MT53XX_COMPRESS = $(TOOL_ROOT)/$(PACK_TOOL_NAME)/lzhsenc $(EXE_NAME).bin $(EXE_NAME).bin.lzhs

#
# Check Fast Build
#
export BRANCH_NAME  = $(word 2,$(BUILD_NAME))
export WARNING_LOG  = $(THIS_ROOT)/build_$(TARGET).log
export ERROR_LOG    = $(THIS_ROOT)/build_fail.log
export CHK_ERR_WARN_SCRIPT = $(TOOL_ROOT)/pbuild/chk_warn_err.sh 
export CHK_ERR_WARN_PARM   = $(TOOL_ROOT)/pbuild $(BRANCH_NAME) $(WARNING_LOG) $(ERROR_LOG)

ifndef FAST_SUPPORT
export FAST_SUPPORT=true
endif

ifneq "$(FAST_SUPPORT)" "false"
MAKE = $(FMAKE)
endif

#
#for archieve library
#
LIB_LIST =	$(wildcard $(AR_LIB_PATH)/*.content)


#
# Build the bits
#
.PHONY : all cfg mw x_inc c_inc custom target image loader packing clean clean_mw clean_x clean_c clean_custom clean_target clean_image clean_force clean_loader clean_brdcst_if clean_sym_link clean_oad init fast fast_mw fast_custom fast_target fast_image fast_packing fast_loader remove_lock target_symlink drv_inc $(LIB_LIST) ar_lib mem_analysis


all : init mw target custom image $(DOLOADER) do_oad mem_analysis


mem_analysis:
ifeq "$(SYS_MEM_CHK)" "true"
	cd $(TOOL_ROOT)/mem_analysis/; \
	perl $(TOOL_ROOT)/mem_analysis/mem_chk_static.pl $(CUSTOMER) $(MODEL_NAME) $(BUILD_CFG)
endif

do_oad :
ifneq "$(CUST_OAD_OUI)" ""
	@if [ -e ./oad_table.mak ]; then \
		for i in $(TS_LIST); do \
			$(MAKE) -f ./oad_table.mak $$i=true; \
		done \
	else \
		if [ -e $(TOOL_ROOT)/gen_oad/oad.sh ]; then \
			/bin/bash $(TOOL_ROOT)/gen_oad/oad.sh; \
		fi; \
	fi
endif

fast : fast_packing
	@echo "BUILD IMAGE"
	$(MAKE) -f all.mak all INC_CFG_MAK=TRUE
			
fast_ice : target mw custom
	@echo "BUILD IMAGE"
	$(MAKE) -f all.mak all INC_CFG_MAK=TRUE

fast_loader : fast_ice
	@echo "BUILD LOADER"
ifeq "$(UBOOT_LOADER)" "TRUE"
	@if [ ! -d $(TARGET_ROOT)/uboot ]; then                             \
		@echo "Please Check out vm_proj_x to get uboot from CVS";    \
		exit;                                                   \
	fi
		@$(MAKE) -C $(TARGET_ROOT)/uboot
else
		@$(MAKE) -C $(TARGET_ROOT)/mtkloader
endif


init: target_symlink remove_lock x_inc c_inc drv_inc
	@echo "FINISH INITIALIZATION"

cfg :
	@chmod +x $(GEN_CFG)
	$(GEN_CFG) -i $(CFG_DEF) -r $(PROJECT_ROOT)

remove_lock : cfg
	@echo "REMOVE LIBRARY LOCKFILE"
	$(call if_dir_exist_fct,$(call path_cnvt,$(ALL_TARGET_LIB_DIR)), find $(call path_cnvt,$(ALL_TARGET_LIB_DIR))/ -name *.lock | xargs $(RM) $(RM_REC_FLAG) $(NULL_DEV);)
	$(call if_dir_exist_fct,$(call path_cnvt,$(ALL_MW_LIB_DIR)),     find $(call path_cnvt,$(ALL_MW_LIB_DIR))/ -name *.lock | xargs $(RM) $(RM_REC_FLAG) $(NULL_DEV);)
	$(call if_dir_exist_fct,$(call path_cnvt,$(ALL_CUSTOM_LIB_DIR)), find $(call path_cnvt,$(ALL_CUSTOM_LIB_DIR))/ -name *.lock | xargs $(RM) $(RM_REC_FLAG) $(NULL_DEV);)
	$(call if_dir_exist_fct,$(call path_cnvt,$(TARGET_ROOT)/$(TARGET_DRIVER)/drv_inc), find $(call path_cnvt,$(TARGET_ROOT)/$(TARGET_DRIVER)/drv_inc)/ -name *.lock | xargs $(RM) $(RM_REC_FLAG) $(NULL_DEV);)
	@$(RM) $(RM_REC_FLAG) $(WARNING_LOG) $(ERROR_LOG)

target_symlink : cfg
	$(MAKE) -C $(TARGET_ROOT) symlink INC_CFG_MAK=TRUE
	
drv_inc :
	@echo "BUILD DRV_INC"
	$(MAKE) -C $(TARGET_ROOT) do_drv_inc INC_CFG_MAK=TRUE

x_inc c_inc : cfg
	@$(MAKE) -C $(MW_ROOT) $@ INC_CFG_MAK=TRUE

fonts : cfg
	@for i in $(CUST_SEL_FONTS); do \
        	$(MAKE) -C $(CUST_SEL_FONTS) all INC_CFG_MAK=TRUE; \
	done

mw : remove_lock
	@echo "BUILD_MW_START_TIME = `$(DATE) $(DATE_FLAG)`"
	@$(MAKE) -C $(MW_ROOT) all INC_CFG_MAK=TRUE; \
	$(call set_build_chk_fct,$(CHK_ERR_WARN_SCRIPT),$(CHK_ERR_WARN_PARM))
	$(call if_file_exist_fct, $(ERROR_LOG), exit 1;)
	@$(MAKE) -f $(TARGET_ROOT)/customer.mak ar_lib AR_LIB_PATH=$(call path_cnvt,$(MW_LIB_DIR))
	@echo "BUILD_MW_END_TIME   = `$(DATE) $(DATE_FLAG)`"

custom : remove_lock fonts c_inc x_inc
	@echo "BUILD_CUSTOM_START_TIME = `$(DATE) $(DATE_FLAG)`"
	@$(MAKE) -C $(CUSTOM_ROOT) all INC_CFG_MAK=TRUE; \
	$(call set_build_chk_fct,$(CHK_ERR_WARN_SCRIPT),$(CHK_ERR_WARN_PARM))
	$(call if_file_exist_fct, $(ERROR_LOG), exit 1;)
	@$(MAKE) -f $(TARGET_ROOT)/customer.mak ar_lib AR_LIB_PATH=$(call path_cnvt,$(CUSTOM_LIB_DIR))
	@echo "BUILD_CUSTOM_END_TIME   = `$(DATE) $(DATE_FLAG)`"


target : remove_lock x_inc
	@echo "BUILD_TARGET_START_TIME = `$(DATE) $(DATE_FLAG)`"
	@$(MAKE) -C $(TARGET_ROOT) symlink sub_comp SUB_COMP="$(TARGET_SUB_COMP_LIST)" INC_CFG_MAK=TRUE; \
	$(call set_build_chk_fct,$(CHK_ERR_WARN_SCRIPT),$(CHK_ERR_WARN_PARM))
	$(call if_file_exist_fct, $(ERROR_LOG), exit 1;)
	@$(MAKE) -f $(TARGET_ROOT)/customer.mak ar_lib AR_LIB_PATH=$(call path_cnvt,$(TARGET_LIB_DIR))
	@echo "BUILD_TARGET_END_TIME   = `$(DATE) $(DATE_FLAG)`"
	
image : remove_lock
	@$(MAKE) -f all.mak all INC_CFG_MAK=TRUE; \
	$(call set_build_chk_fct,$(CHK_ERR_WARN_SCRIPT),$(CHK_ERR_WARN_PARM))

clean : clean_mw clean_custom clean_target clean_image clean_loader clean_pack clean_cfg clean_oad clean_sym_link
	@$(RM) $(RM_REC_FLAG) $(WARNING_LOG) $(ERROR_LOG)

clean_cfg :
	@chmod +x $(GEN_CFG)
	$(GEN_CFG) -c $(CFG_DEF) -r $(PROJECT_ROOT)


clean_all_cfg :
	@chmod +x $(GEN_CFG)
	$(GEN_CFG) -c $(MODEL_NAME).cfg -r $(PROJECT_ROOT)
	$(GEN_CFG) -c $(MODEL_NAME)_cli.cfg -r $(PROJECT_ROOT)
	$(GEN_CFG) -c $(MODEL_NAME)_dbg.cfg -r $(PROJECT_ROOT)


clean_mw :
	@$(MAKE) -s -C $(MW_ROOT) clean INC_CFG_MAK=TRUE

clean_x clean_c :
	@$(MAKE) -s -C $(MW_ROOT) $@ INC_CFG_MAK=TRUE


clean_custom :
	@$(MAKE) -s -C $(CUSTOM_ROOT) clean INC_CFG_MAK=TRUE


clean_target :
	@$(MAKE) -s -C $(TARGET_ROOT) clean INC_CFG_MAK=TRUE


clean_image :
	@$(MAKE) -s -f clean.mak clean INC_CFG_MAK=TRUE
	@if [ -e upg_pkg.mak ]; then 	                        \
		$(MAKE) -s -f upg_pkg.mak clean INC_CFG_MAK=TRUE; \
	fi


clean_all_image :
	@$(MAKE) -s -f clean.mak clean_all INC_CFG_MAK=TRUE
	@if [ -e upg_pkg.mak ]; then 	                        \
		$(MAKE) -s -f upg_pkg.mak clean_all INC_CFG_MAK=TRUE; \
	fi

clean_force :
	@$(MAKE) -s -C $(MW_ROOT) $@ INC_CFG_MAK=TRUE
	@$(MAKE) -s -C $(CUSTOM_ROOT) $@ INC_CFG_MAK=TRUE
	@$(MAKE) -s -C $(TARGET_ROOT) $@ INC_CFG_MAK=TRUE
	@$(MAKE) -s -f clean.mak $@ INC_CFG_MAK=TRUE

clean_all : clean_x clean_c clean_all_image clean_all_loader clean_all_pack clean_all_cfg clean_oad
	@$(RM) $(RM_REC_FLAG) $(call path_cnvt,$(ALL_OBJ_DIR)) $(NULL_DEV)
	@$(RM) $(RM_REC_FLAG) $(call path_cnvt,$(ALL_LIB_LIB_DIR)) $(NULL_DEV)
	@$(RM) $(RM_REC_FLAG) $(call path_cnvt,$(ALL_MW_LIB_DIR)) $(NULL_DEV)
	@$(RM) $(RM_REC_FLAG) $(call path_cnvt,$(ALL_CUSTOM_LIB_DIR)) $(NULL_DEV)
	@$(RM) $(RM_REC_FLAG) $(call path_cnvt,$(ALL_TARGET_LIB_DIR)) $(NULL_DEV)

loader : x_inc c_inc drv_inc
ifeq "$(UBOOT_LOADER)" "TRUE"
	@if [ ! -d $(TARGET_ROOT)/uboot ]; then 	                        \
		echo "Please Check out vm_proj_x to get uboot from CVS";	\
		exit;			                                        \
	fi
	@$(MAKE) -C $(TARGET_ROOT)/uboot
else
	@$(MAKE) -C $(TARGET_ROOT)/mtkloader
endif

clean_oad :
	@echo "Remove oad"
	@$(RM) -r $(RM_FLAG) $(THIS_ROOT)/oad

clean_loader :
ifeq "$(UBOOT_LOADER)" "TRUE"
	@if [ -d $(TARGET_ROOT)/uboot ]; then			\
		$(MAKE) -s -i -C $(TARGET_ROOT)/uboot clean;	\
		echo "$(MAKE) -s -i -C $(TARGET_ROOT)/uboot clean";	\
	fi
else
	@if [ -d $(TARGET_ROOT)/mtkloader ]; then			\
		$(MAKE) -s -i -C $(TARGET_ROOT)/mtkloader clean;	\
		echo "$(MAKE) -s -i -C $(TARGET_ROOT)/mtkloader clean";	\
	fi
endif
	$(RM) $(RM_FLAG) $(EXE_NAME) $(EXE_NAME).bin $(EXE_NAME).bin.gz $(EXE_NAME).bin.lzhs
	$(RM) $(RM_FLAG) $(EXE_NAME)_loader.bin $(MTKTOOL_BIN)
	$(RM) $(RM_FLAG) $(EXE_NAME)_dbg $(EXE_NAME)_dbg.bin $(EXE_NAME)_dbg.bin.gz $(EXE_NAME)_dbg.bin.lzhs
	$(RM) $(RM_FLAG) $(EXE_NAME)_dbg_loader.bin $(MTKTOOL_BIN)

clean_all_loader :
ifeq "$(UBOOT_LOADER)" "TRUE"
	@if [ -d $(TARGET_ROOT)/uboot ]; then			\
		$(MAKE) -s -i -C $(TARGET_ROOT)/uboot clean;	\
		echo "$(MAKE) -s -i -C $(TARGET_ROOT)/uboot clean";	\
	fi
else
	@if [ -d $(TARGET_ROOT)/mtkloader ]; then			\
		$(MAKE) -s -i -C $(TARGET_ROOT)/mtkloader clean;	\
		echo "$(MAKE) -s -i -C $(TARGET_ROOT)/mtkloader clean";	\
	fi
endif
	$(RM) $(RM_FLAG) $(ALL_EXE_NAME) $(foreach i,$(ALL_EXE_NAME),$(i).bin) $(foreach i, $(ALL_EXE_NAME), $(i).bin.gz) $(foreach i,$(ALL_EXE_NAME), $(i).bin.lzhs)
	$(RM) $(RM_FLAG) $(foreach i,$(ALL_EXE_NAME), $(i)_loader.bin) $(ALL_MTKTOOL_BIN)


clean_pack :
ifeq "$(UBOOT_LOADER)" "TRUE"
	$(RM) $(RM_FLAG) $(THIS_ROOT)/$(EXE_NAME)_loader.bin
endif
	$(RM) $(RM_FLAG) $(THIS_ROOT)/$(EXE_NAME)_loader.bin.hdr.bin
	$(MAKE) -C $(TOOL_ROOT)/mt5391_pack INC_PATH=target/$(TARGET)/$(TARGET_DRIVER)/loaderlib TARGET=linux_pc TARGET_ROOT=$(PROJECT_ROOT)/target/linux_pc clean
	$(MAKE) -C $(TOOL_ROOT)/mt5381_pack INC_PATH=target/$(TARGET)/$(TARGET_DRIVER)/loaderlib TARGET=linux_pc TARGET_ROOT=$(PROJECT_ROOT)/target/linux_pc clean

clean_all_pack :
ifeq "$(UBOOT_LOADER)" "TRUE"
	$(RM) $(RM_FLAG) $(foreach i,$(EXE_NAME),$(THIS_ROOT)/$(i)_loader.bin)
endif
	$(RM) $(RM_FLAG) $(foreach i, $(ALL_EXE_NAME),$(THIS_ROOT)/$(i)_loader.bin.hdr.bin) $(THIS_ROOT)/$(EXE_NAME)_loader.bin.hdr.bin
	$(MAKE) -C $(TOOL_ROOT)/mt5391_pack INC_PATH=target/$(TARGET)/$(TARGET_DRIVER)/loaderlib TARGET=linux_pc TARGET_ROOT=$(PROJECT_ROOT)/target/linux_pc clean
	$(MAKE) -C $(TOOL_ROOT)/mt5381_pack INC_PATH=target/$(TARGET)/$(TARGET_DRIVER)/loaderlib TARGET=linux_pc TARGET_ROOT=$(PROJECT_ROOT)/target/linux_pc clean

fast_packing : fast_loader
	$(MAKE) -C $(TOOL_ROOT)/mt5391_pack INC_PATH=target/$(TARGET)/$(TARGET_DRIVER)/loaderlib TARGET=linux_pc TARGET_ROOT=$(PROJECT_ROOT)/target/linux_pc
	$(MAKE) -C $(TOOL_ROOT)/mt5381_pack INC_PATH=target/$(TARGET)/$(TARGET_DRIVER)/loaderlib TARGET=linux_pc TARGET_ROOT=$(PROJECT_ROOT)/target/linux_pc
	$(FROMELF) --bin $(EXE_NAME) --output $(EXE_NAME).bin
	$(RM) $(RM_FLAG) $(EXE_NAME).bin.gz $(EXE_NAME).bin.lzhs $(MTKTOOL_BIN)
	$(MT53XX_COMPRESS)
ifeq "$(UBOOT_LOADER)" "TRUE"
	$(CP) $(CP_FLAG) $(TARGET_ROOT)/uboot/$(EXE_NAME)_loader.bin $(THIS_ROOT)/$(EXE_NAME)_loader.bin
else
	$(CP) $(CP_FLAG) $(TARGET_ROOT)/mtkloader/$(EXE_NAME)_mtkloader.bin $(EXE_NAME)_loader.bin
endif
	$(MT53XX_CMDLINE)
	@if [ -f packing.mak ]; then 								\
		$(MAKE) -f packing.mak; 							\
	else											\
		echo "obase=16;$(LOADER_MAX_SIZE)" > .cal.txt;					\
		echo "quit" >> .cal.txt;							\
		echo "$(MTKTOOL_BIN)	.	." > packing_$(EXE_NAME).scr;			\
		echo -n "$(EXE_NAME)_loader.bin.hdr.bin 0x0 0x" >> packing_$(EXE_NAME).scr;	\
		bc -q .cal.txt >> packing_$(EXE_NAME).scr;					\
		echo -n "$(EXE_NAME).bin.lzhs.hdr 0x" >> packing_$(EXE_NAME).scr;		\
		echo -n `bc -q .cal.txt` >> packing_$(EXE_NAME).scr;				\
		echo " ." >> packing_$(EXE_NAME).scr;						\
		$(MT5391_PACKIMG) -f packing_$(EXE_NAME).scr;					\
		$(RM) $(RM_FLAG) packing_$(EXE_NAME).scr;					\
	fi
	@if [ -f upg_pkg.mak ]; then \
		$(MAKE) -f upg_pkg.mak; \
	fi
	@$(call set_build_chk_fct,$(CHK_ERR_WARN_SCRIPT),$(CHK_ERR_WARN_PARM))
	@echo "BUILD_START_TIME = $(BUILD_START_TIME)"
	@echo "BUILD_END_TIME   = `$(DATE) $(DATE_FLAG)`"	
	@echo "Finished ..."
	
packing :
	$(MAKE) -C $(TOOL_ROOT)/mt5391_pack INC_PATH=target/$(TARGET)/$(TARGET_DRIVER)/loaderlib TARGET=linux_pc TARGET_ROOT=$(PROJECT_ROOT)/target/linux_pc
	$(MAKE) -C $(TOOL_ROOT)/mt5381_pack INC_PATH=target/$(TARGET)/$(TARGET_DRIVER)/loaderlib TARGET=linux_pc TARGET_ROOT=$(PROJECT_ROOT)/target/linux_pc
	$(FROMELF) --bin $(EXE_NAME) --output $(EXE_NAME).bin
	$(RM) $(RM_FLAG) $(EXE_NAME).bin.gz $(EXE_NAME).bin.lzhs $(MTKTOOL_BIN)
	$(MT53XX_COMPRESS)
ifeq "$(UBOOT_LOADER)" "TRUE"
	$(CP) $(CP_FLAG) $(TARGET_ROOT)/uboot/$(EXE_NAME)_loader.bin $(THIS_ROOT)/$(EXE_NAME)_loader.bin
else
	$(CP) $(CP_FLAG) $(TARGET_ROOT)/mtkloader/$(EXE_NAME)_mtkloader.bin $(EXE_NAME)_loader.bin
endif
	$(MT53XX_CMDLINE)
	@if [ -f packing.mak ]; then 								\
		$(MAKE) -f packing.mak; 							\
	else											\
		echo "obase=16;$(LOADER_MAX_SIZE)" > .cal.txt;					\
		echo "quit" >> .cal.txt;							\
		echo "$(MTKTOOL_BIN)	.	." > packing_$(EXE_NAME).scr;			\
		echo -n "$(EXE_NAME)_loader.bin.hdr.bin 0x0 0x" >> packing_$(EXE_NAME).scr;	\
		bc -q .cal.txt >> packing_$(EXE_NAME).scr;					\
		echo -n "$(EXE_NAME).bin.lzhs.hdr 0x" >> packing_$(EXE_NAME).scr;		\
		echo -n `bc -q .cal.txt` >> packing_$(EXE_NAME).scr;				\
		echo " ." >> packing_$(EXE_NAME).scr;						\
		$(MTK_PACKIMG) -f packing_$(EXE_NAME).scr;					\
		$(RM) $(RM_FLAG) packing_$(EXE_NAME).scr;					\
	fi
	@if [ -f upg_pkg.mak ]; then \
		$(MAKE) -f upg_pkg.mak; \
	fi
ifdef SCRIPT_LIST
	for i in $(SCRIPT_LIST); do \
		echo $(MAKE) -f upg_pkg.mak SCRIPT_PROFILE=$(THIS_ROOT)/upgrade_scripts/$$i; \
		$(MAKE) -f upg_pkg.mak SCRIPT_PROFILE=$(THIS_ROOT)/upgrade_scripts/$$i; \
	done
endif
	@$(call set_build_chk_fct,$(CHK_ERR_WARN_SCRIPT),$(CHK_ERR_WARN_PARM))
	@echo "BUILD_START_TIME = $(BUILD_START_TIME)"
	@echo "BUILD_END_TIME   = `$(DATE) $(DATE_FLAG)`"
	@echo "Finished ..."

ar_lib :$(LIB_LIST) 
	@echo "Archieve Library Done"
	
$(LIB_LIST) :
	@/bin/bash $(TOOL_ROOT)/archieve_lib/ar_lib.sh $(AR) $(AR_FLAG) `echo $@|sed -r 's/\.content//'` $@
	@$(RM) $(RM_REC_FLAG) $@


clean_brdcst_if :
	find $(TARGET_ROOT)/obj/ $(TARGET_ROOT)/lib_target/ -name brdcst_if | xargs rm -rf
	find $(TARGET_ROOT)/lib_target/ -name *P_analog* | xargs rm -rf
	find $(TARGET_ROOT)/lib_target/ -name libtuner* | xargs rm -rf

clean_sym_link :
	@echo "Remove Symbolic Link"
	@find $(TARGET_ROOT) -type l -exec stat {} \; |grep "File:"|sed -r "s/\`//g"|sed -r "s/'//g"|grep -v secure|awk '{print $$2}' | xargs rm -rf
help:
	@echo "make"
	@echo "	To build release version without symbol & debug information"
	@echo "make BUILD_CFG=debug"
	@echo "	To build debug version"
	@echo "make help"
	@echo "	This menu"
	@echo "make BRDCST_IF={nim library}"
	@echo "	Default library is $(BRDCST_IF). To link with another nim library."
	@echo -n "	All nim libraries are:"
	@for i in `ls $(TARGET_ROOT)/brdcst_if`; do \
		if [ -f "$(TARGET_ROOT)/brdcst_if/$$i/libtuner.a" ]; then \
			echo -n " $$i"; \
		fi \
	done
	@echo " "

all_lint : mw_lint custom_lint target_lint

mw_lint: cfg
	$(MAKE) -C $(MW_ROOT) lint INC_CFG_MAK=TRUE

custom_lint: fonts x_inc c_inc target_symlink
	$(MAKE) -C $(CUSTOM_ROOT) lint INC_CFG_MAK=TRUE

target_lint: x_inc
	$(MAKE) -C $(TARGET_ROOT) symlink do_drv_inc lint INC_CFG_MAK=TRUE DRV_LINT=TRUE

clean_all_lint: clean_mw_lint clean_custom_lint clean_target_lint

clean_mw_lint:
	$(MAKE) -C $(MW_ROOT) clean_lint INC_CFG_MAK=TRUE

clean_custom_lint:
	$(MAKE) -C $(CUSTOM_ROOT) clean_lint INC_CFG_MAK=TRUE

clean_target_lint:
	$(MAKE) -C $(TARGET_ROOT) clean_lint INC_CFG_MAK=TRUE
