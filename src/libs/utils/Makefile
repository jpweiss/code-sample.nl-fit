# -*- Makefile -*-
# Copyright (C) 2004-2009 by John P. Weiss
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
# RCS $Id: Makefile 1905 2010-02-19 18:59:22Z candide $
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
TARG_LIB:=libutils
TARG_LIB_TYPES:=$(TARG_LIB).a # $(TARG_LIB).so

# C files
CSRC:=

# Standalone Headers or C headers.
#[jpw::subset]HEADERS:=jpw_nld.h nld_exceptions.h \
#[jpw::subset]	Matrix.h MatrixAdapter.h Matrix_fwd.h Vector_fwd.h MathTools.h \
#[jpw::subset]	MatrixIO.h
HEADERS:=jpw_nld.h nld_exceptions.h \
	Matrix.h MatrixAdapter.h Matrix_fwd.h Vector_fwd.h MathTools.h

# Standalone C++ Headers/Template Source.
# Should live under "details" subdir.  Will be installed under
# $(INCDIR)/details, with relative path preserved.
#[jpw::subset]HEADER_DETAILS:=CustomManips.h MatrixRef_Writer.h SequenceStorage.h \
#[jpw::subset]	 ContainerType.h MatrixAdapter.tcc IO_Raw_Base.tcc Matrix.tcc
HEADER_DETAILS:=CustomManips.h MatrixRef_Writer.h SequenceStorage.h \
	 ContainerType.h MatrixAdapter.tcc Matrix.tcc

# C++ files
#[jpw::subset]CXX_SRC:=statistics.cc Manips.cc ConfigFileReader.cc RawIO.cc SushiIO.cc
CXX_SRC:=statistics.cc Manips.cc
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

build_all: $(TARG_LIB_TYPES)

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
