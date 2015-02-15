

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

DRV_ROOT := $(shell if [ -e $(DRV_ROOT)/driver.mak ]; then echo "$(DRV_ROOT)"; else echo "$(PROJECT_ROOT)/target/$(TARGET_IC)"; fi)

else
include $(PROJECT_ROOT)/path.mak
endif


THIS_COMPONENT = SII1292


SRC= 

OBJ = sii1292_fiji_linux_2k13.o 

SUB_COMPONENTS = 

OPTIONAL_SUB_COMPONENTS =

#############################################################################
#
# Include the makefile common to all components
#

ifeq "$(LINUX_SOLUTION)" "true"
include $(DRV_ROOT)/driver.mak
else
include $(TARGET_ROOT)/common.mak
endif

