# Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
#  
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as 
# published by the Free Software Foundation version 2.
# 
# This program is distributed "as is" WITHOUT ANY WARRANTY of any
# kind, whether express or implied; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

top_srcdir		?= ..

include $(top_srcdir)/include/mk/env_pre.mk

INSTALL_DIR		:= platforms

FILTER_OUT_DIRS         := 

# We don't want to copy over the Makefile
UNWANTED_FILES		:= Makefile

INSTALL_MODE		:= 00664

INSTALL_TARGETS		:= $(filter-out $(UNWANTED_FILES),$(notdir $(patsubst $(abs_srcdir)/%,%,$(wildcard $(abs_srcdir)/*))))

MAKE_TARGETS		:=

include $(top_srcdir)/include/mk/generic_leaf_target.mk
