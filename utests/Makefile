# -*- Makefile -*-
# Copyright (C) 2006-2014 by John P. Weiss
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
# Source Variables
#
##########


# Common Variables
#
include make.vars.mk


#[jpw::subsetOnly]TARG_SUBDIRS_INSTALL:=templ.classes trivial.standalone ini.file file.io
TARG_SUBDIRS_INSTALL:=templ.classes
TARG_SUBDIRS:=$(TARG_SUBDIRS_INSTALL)


##########
#
# Make Rules
#
##########


all: build_all

relink: #clean_targs build_all
	-echo "Bulk-Relinking ignored for Unit Tests"

build_all: $(TARG_SUBDIRS:%=%.build_all)

install: $(TARG_SUBDIRS:%=%.install)

reinstall: relink install

run: $(TARG_SUBDIRS:%=%.run)

clean: clean_gcov $(TARG_SUBDIRS:%=%.clean)

clean_binaries:
	-rm bin/*
	-rm lib/lib*.a
	-rm lib/*.so

veryclean: clean_binaries $(TARG_SUBDIRS:%=%.veryclean)

gcov: $(TARG_SUBDIRS:%=%.gcov)


# Remove or alter these as needed.
clean_gcov:
	-find . \( -name '*.gcov' -o  -name '*.gcda' -o  -name '*.gcno' \) \
		-exec rm -v '{}' ';'

distclean: $(TARG_SUBDIRS:%=%.veryclean) clean_binaries
	-find . -name '*.d' -exec rm -v '{}' ';'

$(TARG_SUBDIRS:%=%.build_all):
	cd $(@:%.build_all=%); $(MAKE) build_all

$(TARG_SUBDIRS_INSTALL:%=%.install):
	cd $(@:%.install=%); $(MAKE) install

$(TARG_SUBDIRS_INSTALL:%=%.install_headers):
	cd $(@:%.install_headers=%); $(MAKE) install_headers

$(TARG_SUBDIRS:%=%.run):
	cd $(@:%.run=%); $(MAKE) run

$(TARG_SUBDIRS:%=%.clean):
	cd $(@:%.clean=%); $(MAKE) clean

$(TARG_SUBDIRS:%=%.veryclean):
	cd $(@:%.veryclean=%); $(MAKE) veryclean

$(TARG_SUBDIRS:%=%.gcov):
	cd $(@:%.gcov=%); make gcov

tags: TAGS

TAGS:
	(\
	find . \( \
		`echo -name\ '*.'{h,C,cc,c,sh,p{m,l},js,java}\ -o\ `\
		`echo -name\ '*.'{octave,m,f,xml,tcl,el}\ -o\ `\
		-name '*akefile' -o -name '*.mk' \) -print0 \
	| xargs -0 etags --declarations -d \
	)

#
# Common Rules
#

include $(BASEDIR)/make.miscrules.mk


#################
#
#  End
