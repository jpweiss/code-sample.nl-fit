# -*- Makefile -*-
#
# Basic variables, like names of directories, flags for header/library
# locations, installation directories, and the like.
#
#
# Copyright (C) 2004 by John P. Weiss
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
# RCS $Id: make.vars.mk 1864 2009-08-18 02:50:20Z candide $
##########


# For components, submodules, and other deeply-nested directory structures,
# use this:

ifeq ($(origin PARENT_PATH), undefined)
PARENT_PATH:=.
endif
PARENT_PATH:=../$(PARENT_PATH)
include $(PARENT_PATH)/make.vars.mk


#################
#
#  End
