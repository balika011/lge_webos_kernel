

ifndef PROJECT_ROOT
PROJECT_ROOT := $(word 1, $(subst /project_x/,/project_x /, $(shell pwd)))
endif

ifeq "$(LINUX_SOLUTION)" "true"

-include $(PROJECT_ROOT)/path.mak

ifdef LINUX_DRV_ROOT
export DRV_ROOT = $(LINUX_DRV_ROOT)
else
export DRV_ROOT =
endif

else
include $(PROJECT_ROOT)/path.mak
endif

THIS_COMPONENT = Mhl

SRC = mhl_if.c

OBJ = 

SUB_COMPONENTS = SII1292

OPTIONAL_SUB_COMPONENTS = 

DEFINES +=

ifeq "$(LINUX_SOLUTION)" "true"

CC_INC += -I$(DRV_ROOT)/3rd_party/LINUX/$(COMPANY)/d_inc \
          -I$(DRV_ROOT)/d_inc/$(COMPANY)/ \
          -I$(DRV_ROOT)/$(TARGET_DRIVER)/drv_cust/$(COMPANY) \
          -I$(DRV_ROOT)/d_inc
else
CC_INC += -I$(DRV_ROOT)/$(DRIVER_DIR)/drv_cust/$(3RD_PARTIES) \
          -I$(TARGET_ROOT)/d_inc/$(3RD_PARTIES) \
          -I$(TARGET_ROOT)/3rd_party/$(3RD_PARTIES)/d_inc
endif

#############################################################################
#
# Include the makefile common to all components
# 
ifeq "$(LINUX_SOLUTION)" "true"

include $(DRV_ROOT)/driver.mak
else
include $(TARGET_ROOT)/common.mak
endif
