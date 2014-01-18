# -*- Makefile -*-
# Copyright (C) 2006, 2014 by John P. Weiss
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
#
# RCS $Id: Makefile 1706 2007-10-30 23:11:04Z candide $
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

# Defined by make.vars.mk:   JPWTOOLS_BASE
##JPWTOOLS_LIB=$(JPWTOOLS_BASE)/lib

# Archive basename.
TARPKG_NAME=utests_templt

# Executables
TARG_BINS:=t_matrix
TARG_LIB:=
TARG_COMMON_OBJS:=

# Required libraries.  Need to use delayed-eval.
LIBS=-ljpwTools

# Standalone Headers or C headers.
HEADERS:=

# C files
CSRC:=

# C++ files
CXX_SRC:=$(TARG_COMMON_OBJS:%.o=%.cc)

# Headerless C++ files.
CXX_SRC_NO_H:=
CXX_SRC_NO_H += $(TARG_BINS:%=%.cc)

#
# Auto-generated variables for objects and headers.  Must be included here,
# and no earlier.
# Defines the $(OBJS) variable, which contains a list of all object files.
#
include $(BASEDIR)/make.autogenV.mk


# GNU-make Macros; keep those that you need.
lastword = $(word $(words $(1)),$(1))
lastdir = $(call lastword,$(subst /, ,$(1)))
finder = $(shell find $(1) \( -path "*/CVS" -prune \) -o \! -type d -print)


##########
#
# Make Rules
#
##########


all: build_all

relink: clean_targs build_all

build_all: $(TARG_BINS) # $(TARG_LIB).a $(TARG_LIB).so

$(TARG_BINS): % : %.o $(TARG_COMMON_OBJS) libjpwTools.a
	$(CXX) $(LDFLAGS) -o $@ $@.o $(TARG_COMMON_OBJS) $(LIBS)

##libjpwTools.a:
##	ln -s $(JPWTOOLS_LIB)/libjpwTools.a ./


#
# Unit-Testing
#

run_utests: $(TARG_BINS)
	for b in $(TARG_BINS); do \
		case $$b in \
			/*) $$b; ;; \
			*)  ./$$b; ;; \
		esac; \
	done

#
# Profiling
#

gcov: $(SRC)
	for f in $?; do \
		$(GCOV) $(GCOV_OPTS) $$f; \
	done

#
# Common Rules
#

include $(BASEDIR)/make.miscrules.mk

#
# Dependencies
#
include $(BASEDIR)/make.deprules.mk

#T# Manual Deps
#T# You should need these very, very rarely.
#$(SOME_OBJS): $($@:.o=.srcsuf)


# Cleanup
#
include $(BASEDIR)/make.cleanup.mk


#################
#
#  End
