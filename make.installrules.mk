# -*- Makefile -*-
#
# Special GNU-make file of installation rules.
#
#
# Copyright (C) 2007 by John P. Weiss
#
# This package is free software; you can redistribute it and/or modify
# it under the terms of the Artistic License, included as the file
# "LICENSE" in the source code archive.
#
# This package is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# You should have received a copy of the file "LICENSE", containing
# the License John Weiss originally placed this program under.
#
# RCS $Id: make.installrules.mk 1864 2009-08-18 02:50:20Z candide $
##########

# Expected vars:

# The installation directories:
#   BINDIR
#   LIBDIR
#   INCDIR

# List of C/C++ headers to "install" by symlink
#   HEADER_INSTALLABLES

# Target library
#   TARG_LIB

# Target Binaries
#   TARG_BINS


#
# The Rules
#


$(BINDIR):
	mkdir -p $(BINDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

$(INCDIR):
	mkdir -p $(INCDIR)

# Header Installation Rules.
install_headers: $(INCDIR) install_headers.symlink

install_headers.copy: $(HEADER_INSTALLABLES)
	@echo "Installing headers from \"${PWD}\""
	@if [ -n "$?" ]; then \
		cp -dP --parents $? $(INCDIR)/; \
	fi

install_headers.symlink: $(HEADER_INSTALLABLES)
	@echo "Installing headers from \"${PWD}\" (via symlink)"
	@for targ in $?; do \
		link=$(INCDIR)/$${targ#/}; \
		linkPath=`dirname $${link}`; \
		if [ ! -d $${linkPath} ]; then \
			mkdir -p $${linkPath} || break; \
		fi; \
		if [ ! -L $${link} ]; then \
			ln -s $${PWD}/$${targ} $${link} || break; \
		fi; \
	done

# Installation rules for binaries.
install_bin: $(BINDIR) install_bin.parts

install_bin.parts: $(TARG_BINS)
	@echo "Installing binaries from \"${PWD}\""
	@if [ -n "$?" ]; then \
		cp -a $? $(BINDIR)/; \
	fi

# Installation rules for libs.
install_libs.parts.static: $(TARG_LIB).a
	@echo "Installing static libraries  from \"${PWD}\""
	@if [ -n "$(TARG_LIB)" ]; then \
		cp -a $? $(LIBDIR)/; \
	fi

install_libs.parts.dynamic: $(TARG_LIB).so
	@echo "Installing dynamic-link libraries  from \"${PWD}\""
	@if [ -n "$(TARG_LIB)" ]; then \
		cp -a $? $(LIBDIR)/; \
	fi

install_static_libs: $(LIBDIR) install_libs.parts.static

install_dynamic_libs: $(LIBDIR) install_libs.parts.dynamic

install_libs: install_static_libs install_dynamic_libs


#################
#
#  End
