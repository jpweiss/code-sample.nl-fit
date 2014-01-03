# -*- Makefile -*-
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
# RCS $Id: Makefile 1840 2009-04-14 12:37:23Z candide $
##########
#
# Initial includes
#
##########


include make.vars.mk
include $(BASEDIR)/make.syscfg.mk


##########
#
# Source Variables
#
##########

DOXYGEN_CFG=

# Executables
TARG_LIB:=libmeasure
TARG_LIB_TYPES:=$(TARG_LIB).a # $(TARG_LIB).so

# C files
CSRC:=

# Standalone Headers or C headers.
HEADERS:=FitLM_BarrierAdapter.h FitGA.h

# Standalone C++ Headers/Template Source.
# Should live under "details" subdir.  Will be installed under
# $(INCDIR)/details, with relative path preserved.
HEADER_DETAILS:=BarrierModels.tcc FitGA.tcc

# C++ files
#[jpw::subset]CXX_SRC:=BarrierMeasure.cc BarrierModels.cc FitBarrier.cc Confidence.cc
CXX_SRC:=BarrierModels.cc
# Headerless C++ files.
CXX_SRC_NO_H:=

#
# Auto-generated variables for objects and headers.  Must be included here,
# and no earlier.
# Defines the $(OBJS) variable, which contains a list of all object files.
#
include $(BASEDIR)/make.autogenV.mk


##########
#
# Make Rules
#
##########


all: build_all

relink: clean_targs build_all

build_all: install_headers $(TARG_LIB_TYPES)

# GNU Make:
# Use a special builtin pattern rule.
$(TARG_LIB).a: $(TARG_LIB).a($(OBJS))

$(TARG_LIB).so: $(OBJS)
	$(CXX) $(LDFLAGS) -shared -fPIC -o $@ $(OBJS) $(LIBS)


# Installation rules.
#
include $(BASEDIR)/make.installrules.mk

install:  install_headers install_static_libs


# Profiling
#
gcov: $(SRC)
	for f in $?; do \
		$(GCOV) $(GCOV_OPTS) $$f; \
	done


# Dependencies
#
include $(BASEDIR)/make.deprules.mk


# Cleanup
#
include $(BASEDIR)/make.cleanup.mk


#################
#
#  End
