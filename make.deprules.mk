# -*- Makefile -*-
#
# Special GNU-make file with the dependency-building and reading rules.
#
#
# Copyright (C) 2006 by John P. Weiss
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
# RCS $Id: make.deprules.mk 1864 2009-08-18 02:50:20Z candide $
##########


# Expected vars:
#   CSRC

# The following variables created by including "make.autogenV.mk":
#   CXX_SRC_C
#   CXX_SRC_cc
#   CXX_SRC_cpp


#
# The Rules
#


define COMPILE_C.d
set -e; $(CC) $(C_DEPFLAGS) $(CPPFLAGS) -c $< | \
    sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
[ -s $@ ] || rm -f $@
endef

%.d: %.c
	$(COMPILE_C.d)

%.d: %.C
	$(COMPILE_C.d)

%.d: %.cc
	$(COMPILE_C.d)

%.d: %.cpp
	$(COMPILE_C.d)

-include $(CSRC:.c=.d)
-include $(CXX_SRC_C:.C=.d) $(CXX_SRC_cc:.cc=.d) $(CXX_SRC_cpp:.cpp=.d)


#################
#
#  End
