# -*- Makefile -*-
# Copyright (C) 2004-2007 by John P. Weiss
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
# RCS $Id: Makefile 1864 2009-08-18 02:50:20Z candide $
##########
#
# Initial includes
#
##########


include make.vars.mk
include $(BASEDIR)/make.syscfg.mk

# FIXME:  Needed for TARG_ARCH==pentium-m, which is a gcc-v4 option.  However,
# g77 is a gcc-v3.2 compiler.
ifneq (,$(findstring $(F77), g77))
 ifeq ($(ARCH), pentium-m)
  TARGET_ARCH:=-march=pentium3
 endif
endif


##########
#
# Source Variables
#
##########

DOXYGEN_CFG=

# Executables
TARG_LIB:=libfortlib
TARG_LIB_TYPES:=$(TARG_LIB).a # $(TARG_LIB).so

# FORTRAN files
FSRC:=lmder1.f chkder.f gaussj.f dpsort.f svdcmp.f pythag.f \
		ludcmp.f lubksb.f dgetrf.f dgetri.f blas.f

# C files
CSRC:=

# Standalone Headers or C headers.
HEADERS:=FORTTypes.h FORTLib.h FitLM_Adapter.h

# Standalone C++ Headers/Template Source.
# Should live under "details" subdir.  Will be installed under
# $(INCDIR)/details, with relative path preserved.
HEADER_DETAILS:=

# C++ files
CXX_SRC:=FitLM.cc
# Headerless C++ files.
CXX_SRC_NO_H:=MatrixInverters.cc Sorters.cc

#
# Auto-generated variables for objects and headers.  Must be included here,
# and no earlier.
# Defines the $(OBJS) variable, which contains a list of all object files.
#
include $(BASEDIR)/make.autogenV.mk

#
# Additional autogen vars for the FORTRAN sources.
#
SRC += $(FSRC)
FOBJS:=$(FSRC:%.f=%.o)
OBJS += $(FOBJS)


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

# Fortran dependencies must be done by hand.
$(FOBJS): $($@:.o=.f)


# Cleanup
#
include $(BASEDIR)/make.cleanup.mk


#################
#
#  End
