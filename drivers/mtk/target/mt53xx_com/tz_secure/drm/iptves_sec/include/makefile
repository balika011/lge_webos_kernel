ROOT	:= ../..
include	$(ROOT)/makefile.env
include	$(SRCDIR)/makefile.local
VPATH	= $(CURDIR)

LIBNAME	=
SRCS	=
HDRS	= po_basictypes.h po_basictypes_.h
LCLHDRS	= basic_types.h basic_types_.h

install: install-hdr
build all build-all: install
clean distclean check clean-all distclean-all check-all depend indent:
	@echo "Nothing to do"

INSTALLINCDIR    = $(INSTALLDIR)/include
INSTALLINCLCLDIR = $(INSTALLDIR)/include/local

install-hdr: $(INSTALLINCDIR) $(HDRS)
	$(INSTALL) $(HDRS) $(INSTALLINCDIR)/

$(INSTALLINCDIR):
	mkdir -p $@
