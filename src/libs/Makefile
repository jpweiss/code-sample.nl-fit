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
# Source Variables
#
##########


# Common Variables
#
include make.vars.mk


#[jpw::subset]TARG_SUBDIRS_INSTALL:=utils rklib lyapunov fortlib persistence \
#[jpw::subset]	measure dynSysCommon
TARG_SUBDIRS_INSTALL:=utils fortlib persistence measure
TARG_SUBDIRS:=$(TARG_SUBDIRS_INSTALL)


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

$(TARG_SUBDIRS_INSTALL:%=%.install):
	cd $(@:%.install=%); $(MAKE) install

$(TARG_SUBDIRS_INSTALL:%=%.install_headers):
	cd $(@:%.install_headers=%); $(MAKE) install_headers

$(TARG_SUBDIRS:%=%.clean):
	cd $(@:%.clean=%); $(MAKE) clean

$(TARG_SUBDIRS:%=%.veryclean):
	cd $(@:%.veryclean=%); $(MAKE) veryclean

$(TARG_SUBDIRS:%=%.gcov):
	cd $(@:%.gcov=%); make gcov

#
# Common Rules
#

include $(BASEDIR)/make.miscrules.mk


#
# Subdir Dependencies
#


fortlib.*: utils.install_headers

persistence.*: utils.install_headers fortlib.install_headers

measure.*: persistence.install_headers


#################
#
#  End
