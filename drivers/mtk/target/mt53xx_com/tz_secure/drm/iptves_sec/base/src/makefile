ROOT	:= ../../../..
include	$(ROOT)/makefile.env
include	$(SRCDIR)/makefile.local
VPATH	= $(CURDIR)

LIBNAME	= base
SRCS	= base.c mem.c fio.c mutex.c
ifneq ($(PublishBuild),true)
SRCS	+= dbg.c prof.c
endif

include	$(CONFIGDIR)/makefile.rules.lib
