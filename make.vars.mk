# -*- Makefile -*-
#
# Basic variables, like names of directories, flags for header/library
# locations, installation directories, and the like.
#
#
# Copyright (C) 2004-2014 by John P. Weiss
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
# RCS $Id: make.vars.mk 1864 2009-08-18 02:50:20Z candide $
##########


# For the version of this file in $(BASEDIR), use:
ifeq ($(origin PARENT_PATH), undefined)
#BASEDIR:=.
#gnu-make#
BASEDIR:=$(shell pwd)
#sh#BASEDIR:=${PWD}
else
BASEDIR:=$(PARENT_PATH)
#sh#BASEDIR:=${PWD}/$(PARENT_PATH)
endif

# These 4 should never be changed.
SRCDOCDIR_rel:=srcdoc
BINDIR_rel:=bin
INCDIR_rel:=include
LIBDIR_rel:=lib

# These must all be delayed-eval, or we'll grab the wrong value of $(BASEDIR)
ifeq ($(origin INSTALLROOT), undefined)
SRCDOCDIR=$(BASEDIR)/$(SRCDOCDIR_rel)
BINDIR=$(BASEDIR)/$(BINDIR_rel)
INCDIR=$(BASEDIR)/$(INCDIR_rel)
LIBDIR=$(BASEDIR)/$(LIBDIR_rel)
else
# A relative $(INSTALLROOT) causes no end of headaches (the installation may
# occur from a Makefile another 3 levels deep.  That's no problem in the
# $(BASEDIR) case, since each Makefile recomputes $(BASEDIR).  No such thing
# happens for $(INSTALLROOT), however.
ifneq (,$(filter .%,$(INSTALLROOT)))
$(error 'INSTALLROOT' must be an absolute path.  \
  Relative paths will fail)
endif
SRCDOCDIR=$(INSTALLROOT)/$(SRCDOCDIR_rel)
BINDIR=$(INSTALLROOT)/$(BINDIR_rel)
INCDIR=$(INSTALLROOT)/$(INCDIR_rel)
LIBDIR=$(INSTALLROOT)/$(LIBDIR_rel)
endif

#
# Commonly-used GNU-make Macros
#

SCM_DIRS:=-path "*/.svn" -o -path "*/.git" -o -path "*/CVS"
finder = $(shell find $(1) \( $(SCM_DIRS) -prune \) -o \! -type d -print)
lastword = $(word $(words $(1)),$(1))
lastdir = $(call lastword,$(subst /, ,$(1)))

#
# Local Customizations
#

#CPPFLAGS += -I $(BOOST)
CPPFLAGS += $(CPPFLAGS_l) # Add local flags for later eval.
CPPFLAGS += -I . -I $(BASEDIR) -I $(INCDIR)

LDFLAGS += $(LDFLAGS_l) # Add local flags for later eval.
LDFLAGS += -L$(LIBDIR) -L.

LIBS=


#################
#
#  End
