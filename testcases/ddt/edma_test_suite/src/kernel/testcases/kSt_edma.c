/*
 * kSt_edma.c
 *
 * This file demonstrates the simple edma mem to mem copy
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


/*Testcode related header files */
#include "kSt_edma.h"



/*
 * Function            		- kSt_edma_memtomemcpytest
 * Functionality      		- This function recieves the EDMA params and performs mem to mem
 *						copy with DMA channel
 * Input Params   	 	-  acnt,bcnt,ccnt,sync_mode,event_queue
 * Return Value        	 -0 - Succes, -1 - Failure
 * Note                 		-  None
 */

int kSt_edma_memtomemcpytest(int acnt, int bcnt, int ccnt, int sync_mode,
			      int event_queue)
{
	int result = 0;
	unsigned int dma_ch = 0;
	unsigned int tcc = ST_EDMA_TCC_ANY;
	int i;
	int count = 0;
	unsigned int Istestpassed = 0u;
	unsigned int numenabled = 0;
	unsigned int BRCnt = 0;
	int srcbidx = 0;
	int desbidx = 0;
	int srccidx = 0;
	int descidx = 0;
	st_edma_param_set param_set;
	s32 trial =0;
	u32 sample_Time[MAX_TRIALS] = {0,};
	u32 total_Time = 0;

	for (trial = 0; trial <= MAX_TRIALS; trial++)
	{
		
		/* Initalize source and destination buffers */
		for (count = 0u; count < (acnt * bcnt * ccnt); count++) {
			dmabufsrc1[count] = 'A' + (count % 26);
			dmabufdest1[count] = 0;
		}

		/* Set B count reload as B count. */
		BRCnt = bcnt;

		/* Setting up the SRC/DES Index */
		srcbidx = acnt;
		desbidx = acnt;
		if (sync_mode == ASYNC) {
			/* A Sync Transfer Mode */
			srccidx = acnt;
			descidx = acnt;
			
			result = kSt_davinci_request_dma(ST_EDMA_CHANNEL_ANY, "A-SYNC_DMA0",
						     kSt_callback1, NULL,&dma_ch, &tcc, event_queue);

		} else if (sync_mode == ABSYNC) {
			/* AB Sync Transfer Mode */
			srccidx = acnt * bcnt;
			descidx = acnt * bcnt;
			result = kSt_davinci_request_dma(ST_EDMA_CHANNEL_ANY, "AB-SYNC_DMA0",
						     kSt_callback1, NULL,&dma_ch, &tcc, event_queue);
		} else {
			TEST_PRINT_ERR(" Invalid Transfer mode ");
		}
		if (FAILURE == result) {
			TEST_PRINT_ERR("edma_test::davinci_request_dma failed for dma_ch, error:%d",result);
			return result;
		}
		kSt_davinci_set_dma_src_params(dma_ch, (unsigned long)(dmaphyssrc1),
					   INCR, W8BIT);

		kSt_davinci_set_dma_dest_params(dma_ch, (unsigned long)(dmaphysdest1),
					    INCR, W8BIT);

		kSt_davinci_set_dma_src_index(dma_ch, srcbidx, srccidx);

		kSt_davinci_set_dma_dest_index(dma_ch, desbidx, descidx);

		if (sync_mode == ASYNC) {
			/* A Sync Transfer Mode */
			kSt_davinci_set_dma_transfer_params(dma_ch, acnt, bcnt, ccnt, BRCnt, ASYNC);
	        } else if (sync_mode == ABSYNC) {
			/* AB Sync Transfer Mode */
			kSt_davinci_set_dma_transfer_params(dma_ch, acnt, bcnt, ccnt, BRCnt, ABSYNC);
		} else {
			TEST_PRINT_ERR(" Invalid Transfer mode \n");
        	}

		/* Enable the Interrupts on Channel 1 */
		kSt_davinci_get_dma_params(dma_ch, &param_set);
		param_set.opt |= (1 << ITCINTEN_SHIFT);
		param_set.opt |= (1 << TCINTEN_SHIFT);
		param_set.opt |= EDMA_TCC(EDMA_CHAN_SLOT(dma_ch));
		kSt_davinci_set_dma_params(dma_ch, &param_set);

		if (sync_mode == ASYNC) {
			numenabled = bcnt * ccnt;
		} else if (sync_mode == ABSYNC) {
			numenabled = ccnt;
		} else {
			TEST_PRINT_ERR("Invalid Transfer mode \n");
		}
		if (performance == 1)	
			start_timer();

		for (i = 0; i < numenabled; i++) {
			irqraised1 = 0;

			/* Now enable the transfer as many times as calculated above. */
			result = kSt_davinci_start_dma(dma_ch);
			if (result != 0) {
				TEST_PRINT_ERR("edma_test: kSt_davinci_start_dma failed ");
				break;
			}

			/* Wait for the Completion ISR. */
			while (irqraised1 == 0u) ;

			/* Check the status of the completed transfer */
			if (irqraised1 < 0) {
				/* Some error occured, break from the FOR loop. */
				TEST_PRINT_ERR("edma_test: Event Miss Occured!!!");
				break;
			}
		}

		if (performance == 1)	
			sample_Time[trial] = stop_timer();

		if (0 == result) {
			for (i = 0; i < (acnt * bcnt * ccnt); i++) {
				if (dmabufsrc1[i] != dmabufdest1[i]) {
					TEST_PRINT_ERR(" edma_test: "
						"Data write-read matching failed at = %u",i);
					Istestpassed = 0u;
					result = -1;
					break;
				}
			}
			if (i == (acnt * bcnt * ccnt)) {
				Istestpassed = 1u;
			}

			kSt_davinci_stop_dma(dma_ch);
			kSt_davinci_free_dma(dma_ch);
		}

	}	
	if(performance) {
		total_Time = 0;

		for (trial = 0; trial < MAX_TRIALS; trial++)
		{
			total_Time += sample_Time[trial];
		}
	}
	if (Istestpassed == 1u) {
		DEBUG_PRINT("edma_test: Transfer controller/event_queue: %d", event_queue);
		DEBUG_PRINT("edma_test: Mode: %d  0 -> ASYNC, 1 -> ABSYNC", sync_mode);		
		if (performance == 1) { 

			TEST_PRINT_TRC("edma_test: Time Elapsed in usec: %d", (total_Time/MAX_TRIALS));
		} else {
			TEST_PRINT_TRC("edma_test: EDMA Data Transfer Successfull on TC %d",event_queue);
		}
	} else {
		TEST_PRINT_ERR("edma_test: EDMA Data Transfer Failed ");
	}

	return result;
}



