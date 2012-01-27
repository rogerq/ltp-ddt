/*
 * kSt_edma_common.h
 *
 * header file accessed by the edma parser
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed <93>as is<94> WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _KST_EDMA_COMMON_H_
#define _KST_EDMA_COMMON_H_

#include <mach/io.h>
#include <mach/irqs.h>
#include <mach/hardware.h>
//#include <mach/memory.h>
#include "kSt_edma_interface.h"

#define ST_EDMA_TCC_ANY 0
#define ST_DAVINCI_EDMA_PARAM_ANY EDMA_SLOT_ANY
#define ST_EDMA_CHANNEL_ANY EDMA_CHANNEL_ANY
#define ST_QDMA_CHANNEL_ANY 0

#define ST_DMA_COMPLETE DMA_COMPLETE
#define ST_DMA_EVT_MISS_ERROR DMA_CC_ERROR
#define ST_QDMA_EVT_MISS_ERROR 0



typedef struct edmacc_param st_edma_param_set;

int kSt_davinci_request_dma(int dev_id, const char *dev_name,
			void (*callback)(unsigned lch, u16 ch_status, void *data),
			void *data, int *lch, int *tcc,enum dma_event_q eventq_no);

void kSt_davinci_set_dma_src_params(int lch, u32 src_port,
						   enum address_mode mode, enum fifo_width width);

void kSt_davinci_set_dma_dest_params(int lch, u32 dest_port,
						   enum address_mode mode, enum fifo_width width);

void kSt_davinci_set_dma_src_index(int lch, u16 src_bidx, u16 src_cidx);

void kSt_davinci_set_dma_dest_index(int lch, u16 dest_bidx, u16 dest_cidx);

void kSt_davinci_set_dma_transfer_params(int lch, u16 acnt, u16 bcnt, u16 ccnt,
				    u16 bcntrld, enum sync_dimension sync_mode);

void kSt_davinci_get_dma_params(int lch, st_edma_param_set *param_set);

void kSt_davinci_set_dma_params(int lch, st_edma_param_set *param_set);

int kSt_davinci_start_dma(int lch);

void kSt_davinci_stop_dma(int lch);

void kSt_davinci_free_dma(int lch);

void kSt_davinci_dma_link_lch(int lch_head, int lch_queue);

#endif  /*_KST_EDMA_COMMON_H_*/


