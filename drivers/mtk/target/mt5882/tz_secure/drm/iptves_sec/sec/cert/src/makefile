ROOT	:= ../../../..
include	$(ROOT)/makefile.env
include	$(SRCDIR)/makefile.local
VPATH	= $(CURDIR)

LIBNAME	= cert
SRCS	= cert_parse.c cert_verify.c crl_parse.c crl_verify.c cert.c
ifneq ($(BUILD_LEVEL),0)
SRCS	+= cert_inf.c crl_inf.c
endif

include	$(CONFIGDIR)/makefile.rules.lib
