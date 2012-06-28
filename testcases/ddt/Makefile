#
#    testcases Makefile.
#
#    Copyright (C) 2009, Cisco Systems Inc.
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# Garrett Cooper, July 2009
#

top_srcdir		?= ../..

include $(top_srcdir)/include/mk/env_pre.mk

# Define directories to exclude from build/installation
#FILTER_OUT_DIRS		:= kdump

ifeq ($(PLATFORM),dm6467-evm)
FILTER_OUT_DIRS			+= fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
endif

ifeq ($(PLATFORM),dm6446-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
endif

ifeq ($(PLATFORM),dm368-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
endif

ifeq ($(PLATFORM),dm365-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
endif

ifeq ($(PLATFORM),dm355-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
endif

ifeq ($(PLATFORM),am180x-evm)
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
endif

ifeq ($(PLATFORM),am181x-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
endif

ifeq ($(PLATFORM),am335x-evm)
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
endif

ifeq ($(PLATFORM),beaglebone)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
endif

ifeq ($(PLATFORM),am3517-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += edma_test_suite
endif

ifeq ($(PLATFORM),am37x-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
FILTER_OUT_DIRS                 += edma_test_suite
endif

ifeq ($(PLATFORM),am387x-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
endif

ifeq ($(PLATFORM),dm385-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
endif

ifeq ($(PLATFORM),dm813x-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
endif

ifeq ($(PLATFORM),am389x-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
endif

ifeq ($(PLATFORM),am45x-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
endif

ifeq ($(PLATFORM),beagleboard)
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += edma_test_suite
endif

ifeq ($(PLATFORM),c6a814x-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
endif

ifeq ($(PLATFORM),c6a816x-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
endif

ifeq ($(PLATFORM),dm37x-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
endif

ifeq ($(PLATFORM),da850-omapl138-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
endif

ifeq ($(PLATFORM),da830-omapl137-evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
endif

ifeq ($(PLATFORM),omap3evm)
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
FILTER_OUT_DIRS                 += edma_test_suite
endif

ifeq ($(PLATFORM),tci6614-evm)
FILTER_OUT_DIRS                 += edma_test_suite
FILTER_OUT_DIRS                 += fbdev_display_test_suite
FILTER_OUT_DIRS                 += mc_test_suite
FILTER_OUT_DIRS                 += v4l2_capture_test_suite
FILTER_OUT_DIRS                 += v4l2_display_test_suite
FILTER_OUT_DIRS                 += v4l2_display_davinci_test_suite
endif

include $(top_srcdir)/include/mk/generic_trunk_target.mk
