all:
clean:
sub_comp:
x_inc:
c_inc:


###########################################################################
#
#  Copyright (c) 2009, Sony
#  All rights reserved
#
#  Unauthorized use, practice, perform, copy, distribution, reproduction,
#  or disclosure of this information in whole or in part is prohibited.
###########################################################################
# $RCSfile: Makefile,v $
# $Revision: #1 $
# $Date: 2009/06/30 $
# $Author: kl-lees $
#
# Description:
#         Leave-level makefile to build the subcomponent of driver library.
#
#         Specify the source files to be compile in SRC.
#############################################################################

ifndef PROJECT_ROOT
PROJECT_ROOT := $(word 1, $(subst /project_x/,/project_x /, $(shell pwd)))
endif

-include $(PROJECT_ROOT)/path.mak

ifdef LINUX_DRV_ROOT
export DRV_ROOT = $(LINUX_DRV_ROOT)
else
export DRV_ROOT = ../../../../../../..
endif

THIS_COMPONENT = temp_sensor


SRC = mitsumi_temp_sensor.c temp_setup.c

OBJ =

SUB_COMPONENTS =

OPTIONAL_SUB_COMPONENTS =

CC_INC += -I$(DRV_ROOT)/3rd_party \
          -I$(DRV_ROOT)/3rd_party/LINUX/Sony/ATV/priv_drv/sony_drv_inc \
          -I$(DRV_ROOT)/3rd_party/LINUX/Sony/ATV/priv_drv/sony_priv_inc
#############################################################################

#
# To support executing 'make' in the local subdirectory, we need to determine
# some additional information.  The 'Makefile' is more complicate, but it
# is nicer from the developer's perspective because you can run make while in
# the directory you are working in.
#

#
# Include the target specific compiler tool and their options
#
include $(DRV_ROOT)/driver.mak
