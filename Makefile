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
# RCS $Id: Makefile 1864 2009-08-18 02:50:20Z candide $
##########
#
# Source Variables
#
##########


TARPKG_NAME:=research-nld
TARG_SUBDIRS_INSTALL:=src
TARG_SUBDIRS:=$(TARG_SUBDIRS_INSTALL) utests


##########
#
# Make Rules
#
##########


all: build_all_installable

relink: $(TARG_SUBDIRS:%=%.relink_all)

build_all: $(TARG_SUBDIRS:%=%.build_all)

build_all_installable: $(TARG_SUBDIRS_INSTALL:%=%.build_all)

install: $(TARG_SUBDIRS_INSTALL:%=%.install)

reinstall: relink install

clean: clean_gcov $(TARG_SUBDIRS:%=%.clean)

clean_binaries:
	-rm bin/*
	-rm lib/lib*.a
	-rm lib/*.so

veryclean: clean_binaries $(TARG_SUBDIRS:%=%.veryclean)

gcov: $(TARG_SUBDIRS:%=%.gcov)

clean_gcov:
	-find . \( -name '*.gcov' -o  -name '*.gcda' -o  -name '*.gcno' \) \
		-exec rm -v '{}' ';'

distclean: $(TARG_SUBDIRS:%=%.veryclean) clean_binaries
	-find . -name '*.d' -exec rm -v '{}' ';'

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

utests: utests.build_all

utests.run: utests
	cd utests; $(MAKE) run

clean_tags:
	-rm -f TAGS

tags: clean_tags TAGS

TAGS:
	( \
	local nameopts="-name '*akefile'"; \
    for suf in h C cc tcc c sh pm pl js java octave m f xml tcl el mk; do \
		nameopts="$$nameopts$${nameopts:+ -o} -name '*.$$suf'"; \
	done; \
	eval "find . -type f -a \( $$nameopts \) -print0" \
	    | xargs -0 etags -a --members --ignore-indentation --declarations; \
	)

ebrowse:
	(\
	local nameopts=""; \
    for suf in h C cc tcc c; do \
		nameopts="$$nameopts$${nameopts:+ -o} -name '*.$$suf'"; \
	done; \
	eval "find . -type f -a \( $$nameopts \) -print" \
	    | ebrowse --verbose \
	)


#
# Packaging
#


include make.vars.mk
include make.tar_n_doc.mk


#
# Subdir Dependencies
#


# Make sure the unit tests are built after the source is built AND installed.
utests.build_all: src.build_all src.install


#################
#
#  End
