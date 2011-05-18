/*
 * kSt_edma_parser.h
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

#ifndef _KST_EDMA_PARSER_H_
#define _KST_EDMA_PARSER_H_


#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include "kSt_edma.h"

#define EDMA_MINOR 125

static unsigned long edma_status;
#define EDMA_IN_USE        0
#define EDMA_OK_TO_CLOSE   1

#define EDMA_ASYNC 0
#define EDMA_ABSYNC 1
#define EDMA_LINK_ASYNC 6
#define EDMA_LINK_ABSYNC 3
#define EDMA_CHAIN_ASYNC 4
#define EDMA_CHAIN_ABSYNC 5

#define KST_DEVICE_NAME_LEN 50
#define KST_TESTCASE_ID_LEN  50

struct kSt_edma_info {
int acnt;
int bcnt;
int ccnt;
int numtcs;
int perf_flag;
char device[KST_DEVICE_NAME_LEN];
char testcaseid[KST_TESTCASE_ID_LEN];

int ioctl;
}kSt_edma_info;

#define MAX_DMA_TRANSFER_IN_BYTES   10240 * 5

dma_addr_t dmaphyssrc1 = 0;
dma_addr_t dmaphyssrc2 = 0;
dma_addr_t dmaphysdest1 = 0;
dma_addr_t dmaphysdest2 = 0;

char *dmabufsrc1 = NULL;
char *dmabufsrc2 = NULL;
char *dmabufdest1 = NULL;
char *dmabufdest2 = NULL;


int performance = 0;
int numTCs = 2;
int qdma = 0;
int link = 0;
int chain = 0;
int async = 0;
int absync = 0;
int buffer_size = 0;


volatile int irqraised1 = 0;
volatile int irqraised2 = 0;


int kSt_edma_memtomemcpytest(int acnt, int bcnt, int ccnt, int sync_mode,
			      int event_queue);
int kSt_edma3_memtomemcpytest_qdma(int acnt, int bcnt, int ccnt, int sync_mode,
			       int event_queue);
int kSt_edma_memtomemcpytest_link(int acnt, int bcnt, int ccnt, int sync_mode,
				   int event_queue);
int kSt_edma_memtomemcpytest_chain(int acnt, int bcnt, int ccnt, int sync_mode,
				    int event_queue);

#endif  /*_KST_EDMA_PARSER_H_*/

