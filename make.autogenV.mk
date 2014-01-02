# -*- Makefile -*-
#
# Special GNU-make file for auto-generating certain variables.  These
# variables are then used later by special targets in the Makefile.template.
#
#
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
# RCS $Id: make.autogenV.mk 1864 2009-08-18 02:50:20Z candide $
##########

# Expected vars:

# Standalone Headers or C headers.
##HEADERS:=

# Standalone C++ Headers/Template Source.
# Should live under "details" subdir.  Will be installed under
# $(INCDIR)/details, with relative path preserved.
##HEADER_DETAILS:=

# C files
##CSRC:=

# C++ files
##CXX_SRC:=

# Headerless C++ files.
##CXX_SRC_NO_H:=

#
# Auto-generated variables for objects and headers, using GNU-make ":="
# feature.
#

COBJS:=$(CSRC:%.c=%.o)
OBJS:=$(COBJS)
SRC:=$(CSRC)

# Separate out by C++ extension.
CXX_SRC_C:=$(filter %.C,$(CXX_SRC))
CXX_SRC_cc:=$(filter %.cc,$(CXX_SRC))
CXX_SRC_cpp:=$(filter %.cpp,$(CXX_SRC))

# All C++ headers
CXX_H:=$(CXX_SRC_C:%.C=%.h) $(CXX_SRC_cc:%.cc=%.h) $(CXX_SRC_cpp:%.cpp=%.h)

HEADERS += $(CXX_H)

# Now add the headerless C++ files.
CXX_SRC_C += $(filter %.C,$(CXX_SRC_NO_H))
CXX_SRC_cc += $(filter %.cc,$(CXX_SRC_NO_H))
CXX_SRC_cpp += $(filter %.cpp,$(CXX_SRC_NO_H))

# Add all C++ source files to the list of sources.
SRC += $(CXX_SRC_C) $(CXX_SRC_cc) $(CXX_SRC_cpp)

# Create the list of C++ object files, keeping the extensions clear.
CXX_OBJS_C:=$(CXX_SRC_C:%.C=%.o)
CXX_OBJS_cc:=$(CXX_SRC_cc:%.cc=%.o)
CXX_OBJS_cpp:=$(CXX_SRC_cpp:%.cpp=%.o)

# Add C++ objs to list of all objs.
OBJS += $(CXX_OBJS_C) $(CXX_OBJS_cc) $(CXX_OBJS_cpp)
HEADER_INSTALLABLES:=$(HEADERS) $(HEADER_DETAILS:%=details/%)


#################
#
#  End
