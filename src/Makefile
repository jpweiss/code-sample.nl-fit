# -*- Makefile -*-
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
# RCS $Id: Makefile 1706 2007-10-30 23:11:04Z candide $
##########
#
# Source Variables
#
##########


# N.B.: "libs" must come first in this list.
# FIXME:  Change this so that all targ subdirs except "lib" depend on "lib".
# Or something to that effect.
#
# Ideally, the binaries should depend on the libraries that they need, and
# those lib dependencies should build the appropriate library in "libs".  But
# use any old workaround in the meantime.
#[jpw::subsetOnly]TARG_SUBDIRS:=libs dyn.systems persistence-map tools
TARG_SUBDIRS:=libs


##########
#
# Make Rules
#
##########


all: build_all

relink: $(TARG_SUBDIRS:%=%.relink_all)

build_all: $(TARG_SUBDIRS:%=%.build_all)

install: $(TARG_SUBDIRS:%=%.install)

clean: clean_gcov $(TARG_SUBDIRS:%=%.clean)

veryclean: $(TARG_SUBDIRS:%=%.veryclean)

gcov: $(TARG_SUBDIRS:%=%.gcov)

clean_gcov:
	-find . \( -name '*.gcov' -o  -name '*.gcda' -o  -name '*.gcno' \) \
		-exec rm -v '{}' ';'

$(TARG_SUBDIRS:%=%.build_all):
	cd $(@:%.build_all=%); $(MAKE) build_all

$(TARG_SUBDIRS:%=%.relink_all):
	cd $(@:%.relink_all=%); $(MAKE) relink

$(TARG_SUBDIRS:%=%.install):
	cd $(@:%.install=%); $(MAKE) install

$(TARG_SUBDIRS:%=%.clean):
	cd $(@:%.clean=%); $(MAKE) clean

$(TARG_SUBDIRS:%=%.veryclean):
	cd $(@:%.veryclean=%); $(MAKE) veryclean

$(TARG_SUBDIRS:%=%.gcov):
	cd $(@:%.gcov=%); make gcov


#################
#
#  End
