/*
 * kSt_edma.h
 *
 * Common header file accessed by the test code
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


#ifndef _KST_EDMA_H_
#define _KST_EDMA_H_

#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include "kSt_log.h"
#include "kSt_edma_common.h"

#define SUCCESS 		0
#define FAILURE 		-1


#define STATIC_SHIFT                3
#define TCINTEN_SHIFT               20
#define ITCINTEN_SHIFT              21
#define TCCHEN_SHIFT                22
#define ITCCHEN_SHIFT               23

#define MAX_TRIALS 10

extern dma_addr_t dmaphyssrc1 ;
extern dma_addr_t dmaphyssrc2 ;
extern dma_addr_t dmaphysdest1;
extern dma_addr_t dmaphysdest2;

extern char *dmabufsrc1;
extern char *dmabufsrc2;
extern char *dmabufdest1;
extern char *dmabufdest2;

extern int performance;
extern int numTCs;
extern int qdma;
extern int link;
extern int chain;
extern volatile int irqraised1;
extern volatile int irqraised2;

void kSt_callback1(unsigned lch, u16 ch_status, void *data);
void kSt_callback2(unsigned lch, u16 ch_status, void *data);

void start_timer(void);
u32 stop_timer(void);


#endif  /*_KST_EDMA_H_*/


