# -*- Makefile -*-
#
# Special GNU-make file for rules to build source docs and tarballs.
#
#
# Copyright (C) 2006-2010 by John P. Weiss
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
# RCS $Id: make.tar_n_doc.mk 2184 2011-06-03 03:34:13Z candide $
##########

# Expected vars:

# The name of the package/module/archive:
#   TARPKG_NAME

# The installation directories:
#   BINDIR
#   LIBDIR
#   INCDIR
#   SRCDOCDIR
# These 4 vars are usually defined in "make.vars.mk".

# Doxygen-related:
#   DOXYGEN_CFG
#   DOXYGEN_CFG_TARG_DIR


#
# The Rules
#


$(SRCDOCDIR):
	mkdir -p $(SRCDOCDIR)
	[ -n "$(DOXYGEN_CFG_TARG_DIR)" ] && \
		ln -s $(SRCDOCDIR) $(DOXYGEN_CFG_TARG_DIR)

doxy: $(SRCDOCDIR)
	doxygen $(DOXYGEN_CFG)

srcdocs: doxy

tardist: install srcdocs
	@echo "Creating package $(BASEDIR)/$(TARPKG_NAME).tar.gz"
	( \
	cd $(BASEDIR); tarpkg=`pwd`/$(TARPKG_NAME).tar.gz; \
	chmod -R a-w $(SRCDOCDIR); \
	cd $(INSTALLROOT); strip $(BINDIR_rel)/*; \
	cd $(BINDIR); strip *; cd ../; \
	find $(BINDIR_rel) $(LIBDIR_rel) $(INCDIR_rel) \
		\( -path "*/scratch*" -o \
		   -path "*/CVS/*" -o -path "*/.svn/*" \) -prune -o -print \
        | tar -zcf $${tarpkg} -p --files-from=- ; \
	chmod -R +w $(SRCDOCDIR); \
	)

tardist.src: install srcdocs
	@echo "Creating package $(BASEDIR)/$(TARPKG_NAME)-src.tar.gz"
	( \
	chmod -R a-w $(SRCDOCDIR); \
	strip $(BINDIR)/*; \
	cd $(BASEDIR); tarpkg=`pwd`/$(TARPKG_NAME)-src.tar.gz; \
	BASEDIR_rel=`basename $(BASEDIR)`; \
	cd ../; \
	find $${BASEDIR_rel} \
		\( -path "*/scratch*" -o \
		   -path "*/CVS/*" -o -path "*/.svn/*" \) -prune -o \
		\( -name '*.h' -o -name '*.C' -o -name '*.cc' -o -name '*.c'\
			-o -name '*.txt' \
			-o -name '*akefile' -o -name '*akefile.*' -o -name '*.mk' \
			-o -name 'README' -o -name 'LICENSE' -o -name 'WARRANTY' \
			-o -path "*/bin/*" -o -path "*/lib*" \
			-o -path "*html*" -o -path "*rtf*" \) -print \
        | tar -zcf $${tarpkg} -p --files-from=- ; \
	chmod -R +w $(SRCDOCDIR); \
	)


#################
#
#  End
