# -*- Makefile -*-
#
# Special GNU-make file for pseudo-targets and other rules.
#
#
# Copyright (C) 2014 by John P. Weiss
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
# $Id: make.miscrules.mk 2872 2014-01-18 03:55:47Z candide $
##########

# Expected vars:
#
#   TARG_SUBDIRS_INSTALL
#   TARG_SUBDIRS
#
# ...but they're not required.


##########
#
# Variables
#
##########


SUBDIR_BASED_TARGS:=

ifdef TARG_SUBDIRS
SUBDIR_BASED_TARGS += $(TARG_SUBDIRS) $(TARG_SUBDIRS:%=%.relink_all) \
	$(TARG_SUBDIRS:%=%.build_all) $(TARG_SUBDIRS_INSTALL:%=%.install) \
	$(TARG_SUBDIRS:%=%.run) $(TARG_SUBDIRS:%=%.gcov) \
	$(TARG_SUBDIRS:%=%.clean) $(TARG_SUBDIRS:%=%.veryclean)

ifdef TARG_SUBDIRS_INSTALL
SUBDIR_BASED_TARGS += $(TARG_SUBDIRS_INSTALL:%=%.install)
endif # $TARG_SUBDIRS_INSTALL

endif # $TARG_SUBDIRS


##########
#
# The Rules
#
##########


.PHONY: all relink build_all build_all_installable install reinstall \
	install_headers install_headers.symlink install_headers.copy \
	install_bin install_bin.parts \
	install_libs.parts.static install_libs.parts.dynamic \
	install_static_libs install_dynamic_libs install_libs \
	test utest unit_test run_utests run \
	gcov clean_gcov tags clean_tag ebrowse clean_binaries \
	clean clean_targs gprofclean veryclean depclean distclean \
	distclean_toplevel \
	doxy srcdocs tardist tardist.src \
	$(SUBDIR_BASED_TARGS)


#################
#
#  End
