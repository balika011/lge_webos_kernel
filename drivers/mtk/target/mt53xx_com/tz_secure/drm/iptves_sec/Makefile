#####################################################################
#
#	Makefile for HDCP2.x
#
####################################################################

export TOOL_CHAIN ?= 4.5.1
export ENABLE_CA9 ?= true

ifndef VM_LINUX_ROOT
VM_LINUX_ROOT := $(word 1, $(subst /vm_linux/,/vm_linux /, $(shell pwd)))
endif

ifndef CHILING_ROOT
CHILING_ROOT := $(word 1, $(subst /vm_linux/,/vm_linux /, $(shell pwd)))
endif

include $(VM_LINUX_ROOT)/dtv_linux/mak/toolchain.mak
# TOOL_CHAIN=4.5.1_vfp_ca9
ifeq "$(ENABLE_CA9)" "true"
GNUARM_NAME := gnuarm-$(TOOL_CHAIN)_vfp_ca9
else
GNUARM_NAME := gnuarm-$(TOOL_CHAIN)
endif

# ---Third_party path---
THIRD_PARTY_ROOT := $(VM_LINUX_ROOT)/third_party
THIRD_PARTY_SOURCE_PATH := $(THIRD_PARTY_ROOT)/source
THIRD_PARTY_LIB_PATH := $(THIRD_PARTY_ROOT)/library/$(GNUARM_NAME)

# ---Mtk_util path---
MTKUTIL_ROOT := $(VM_LINUX_ROOT)/dtv_linux/mtk_util
MTKUTIL_SOURCE_PATH := $(MTKUTIL_ROOT)/source
MTKUTIL_LIB_PATH := $(MTKUTIL_ROOT)/library/$(GNUARM_NAME)

# ---Oss path---
OSS_ROOT := $(VM_LINUX_ROOT)/oss
OSS_SOURCE_PATH := $(OSS_ROOT)/source
OSS_LIB_PATH := $(OSS_ROOT)/library/$(GNUARM_NAME)


SECURE_STORAGE_INC_PATH = $(MTKUTIL_LIB_PATH)/securestorage/include


OPENSSL_INC_PATH = $(OSS_LIB_PATH)/openssl/1.0.0e

########################################
#  Build options
#######################################
SO_LIB:=true



PWD=$(shell pwd)
TOP_DIR := $(PWD)




# Cipher API from library in user-space or from the driver through MTAL
#CIPHER_API = LIBTOM
CIPHER_API = MTAL

########################################
#   Compiler options/flags
#######################################
SO_CPP_OPT = -shared -fPIC -g -o
INC_FLAGS =  -I$(TOP_DIR)/include  -I$(TOP_DIR)/base/include
			
CFLAGS += -march=armv6 -fsigned-char -fPIC -pipe -msoft-float -Wall -O2 -g
CFLAGS += -Wl,-Map,mapfile
LDFLAGS :=
LIB_PATH_OPT = \
			-L$(MTKUTIL_LIB_PATH)/hdcp/hdcp2.x \
			-L$(OSS_LIB_PATH)/expat/expat_2.0.1/lib
			
			
LIB_OPT = -lpthread -lexpat
LINK_OPT = $(LIB_PATH_OPT) $(LIB_OPT)

ifeq "$(SO_LIB)" "true"
TARGET = libiptves.so
else
TARGET = libiptves.a
endif

##################################################################
# C Flags
#################################################################
SO_LINK_OPT += -L.
#CFLAGS += -DSUPPORT_HDCP2_1


#-------HDCP2.x RX-------


CSOURCES = $(TOP_DIR)/sec/keymng/src/keymng.c \
	$(TOP_DIR)/sec/drm/src/drm.c \
	$(TOP_DIR)/sec/cache/src/cache.c \
	$(TOP_DIR)/sec/sac/src/sac.c \
	$(TOP_DIR)/sec/certmng/src/certmng.c \
	$(TOP_DIR)/sec/sdi/src/sdi.c \
	$(TOP_DIR)/sec/sp/src/sp.c \
	$(TOP_DIR)/sec/asn/src/asn.c \
	$(TOP_DIR)/sec/ecm/src/ecm.c \
	$(TOP_DIR)/sec/nvs/src/nvs.c \
	$(TOP_DIR)/sec/cert/src/cert.c \
	$(TOP_DIR)/sec/cert/src/cert_verify.c \
	$(TOP_DIR)/sec/cert/src/crl_inf.c \
	$(TOP_DIR)/sec/cert/src/crl_verify.c \
	$(TOP_DIR)/sec/cert/src/cert_inf.c \
	$(TOP_DIR)/sec/cert/src/crl_parse.c \
	$(TOP_DIR)/sec/cert/src/cert_parse.c \
	$(TOP_DIR)/base/src/prof.c \
	$(TOP_DIR)/base/src/mutex.c \
	$(TOP_DIR)/base/src/base.c \
	$(TOP_DIR)/base/src/mem.c \
	$(TOP_DIR)/base/src/dbg.c \
	$(TOP_DIR)/base/src/fio.c \
	$(TOP_DIR)/outside/crypto_api_iptves.c \
	$(TOP_DIR)/outside/http_api_iptves.c \
	$(TOP_DIR)/outside/rend_api_iptves.c \
	$(TOP_DIR)/outside/time_api_iptves.c 


INC_FLAGS +=   -I$(TOP_DIR)/sec/keymng/include \
    -I$(TOP_DIR)/sec/drm/include \
    -I$(TOP_DIR)/sec/cache/include \
    -I$(TOP_DIR)/sec/sac/include \
    -I$(TOP_DIR)/sec/certmng/include \
    -I$(TOP_DIR)/sec/sdi/include \
    -I$(TOP_DIR)/sec/sp/include \
    -I$(TOP_DIR)/sec/asn/include \
    -I$(TOP_DIR)/sec/ecm/include \
    -I$(TOP_DIR)/sec/nvs/include \
    -I$(TOP_DIR)/sec/cert/include \
    -I$(TOP_DIR)/base/include \
    -I$(TOP_DIR)/include/outside 


INC_FLAGS += -I$(VM_LINUX_ROOT)/chiling/app_if/mtal/mtal_inc \
			-I$(VM_LINUX_ROOT)/chiling/app_if/mtal/include
LIB_PATH_OPT += -L$(VM_LINUX_ROOT)/chiling/app_if/mtal/lib
LIB_OPT += -lmtal
CFLAGS += -DUSE_MTAL_CIPHER_API
CFLAGS += $(INC_FLAGS)

OBJECTS = $(CSOURCES:.c=.o) $(CXXSOURCES:.cpp=.o)
.PHONY: clean all

all: $(TARGET)
	cp -f $(TARGET) $(MTKUTIL_LIB_PATH)/hdcp/hdcp2.x/

$(TARGET): $(OBJECTS)
ifeq "$(SO_LIB)" "true"
	$(CXX) $(SO_CPP_OPT) $@ $(SO_LINK_OPT) $(OBJECTS)
	$(STRIP) --strip-unneeded *.so
else
	$(AR) r $@ $(OBJECTS)
endif

.c.o:
	$(CC) $(CFLAGS) $< -c -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $< -c -o $@

clean:
	-rm -f $(OBJECTS) $(TARGET)
	
install:

install_basic_lib :
	@echo "Install $(TARGET) to Basic/lib partition"
	cp -f $(MTKUTIL_LIB_PATH)/hdcp/hdcp2.x/*.so $(TARGET_DIR)
    
install_3rd_lib :
	@echo "Install to 3rd/lib partition"       
        
install_basic_bin :
	@echo "Install to basic/bin partitoin"
           
install_3rd_bin :
	@echo "Install to 3rd/bin partitoin"	
                    
install_mtk_util :
	@echo "Install to 3rd/mtk_util"
