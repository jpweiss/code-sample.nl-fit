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
# RCS $Id: Makefile 1840 2009-04-14 12:37:23Z candide $
##########
#
# Source Variables
#
##########


#[jpw::subset]TARG_SUBDIRS:=utils rklib lyapunov fortlib persistence measure dynSysCommon
TARG_SUBDIRS:=utils fortlib persistence measure


##########
#
# Make Rules
#
##########


all: build_all

relink: $(TARG_SUBDIRS:%=%.relink_all)

build_all: $(TARG_SUBDIRS:%=%.build_all)

install: $(TARG_SUBDIRS:%=%.install)

reinstall: relink install

clean: $(TARG_SUBDIRS:%=%.clean)

veryclean: $(TARG_SUBDIRS:%=%.veryclean)

gcov: $(TARG_SUBDIRS:%=%.gcov)

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
