/*
 * kSt_edma_interface.c
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
 * Function            		- kSt_davinci_request_dma
 * Functionality      		- This function acts as an interface for the channel request API
 * Input Params   	 	- dev_id, dev_name, callback, data, lch, tcc, eventq_no
 * Return Value        	 -0 - Succes, -1 - Failure
 * Note                 		-  None
 */

int kSt_davinci_request_dma(int dev_id, const char *dev_name,
			void (*callback)(unsigned lch, u16 ch_status, void *data),
			void *data, int *lch, int *tcc,enum dma_event_q eventq_no)
{

	int result;
	result = edma_alloc_channel(dev_id, callback, data, eventq_no);
	if (result < 0) {
		return FAILURE;
	}
	*lch = result;
	return SUCCESS;
}

/*
 * Function            		- kSt_davinci_set_dma_src_params
 * Functionality      		- This function acts as an interface for the set src parms API
 * Input Params   	 	- lch, src_port, mode, width
 * Return Value        	 -0 - Succes, -1 - Failure
 * Note                 		-  None
 */

void kSt_davinci_set_dma_src_params(int lch, u32 src_port,
			       enum address_mode mode, enum fifo_width width)
{

	edma_set_src((unsigned )lch, (dma_addr_t) src_port,mode, width);

}

/*
 * Function            		- kSt_davinci_set_dma_dest_params
 * Functionality      		- This function acts as an interface for the set dest parms API
 * Input Params   	 	- lch, dest_port, mode, width
 * Return Value        	 -0 - Succes, -1 - Failure
 * Note                 		-  None
 */

void kSt_davinci_set_dma_dest_params(int lch, u32 dest_port,
				enum address_mode mode, enum fifo_width width)
{
	edma_set_dest((unsigned )lch, (dma_addr_t) dest_port, mode, width);
}

/*
 * Function            		- kSt_davinci_set_dma_src_index
 * Functionality      		- This function acts as an interface for the set src index API
 * Input Params   	 	- lch, src_bidx, src_cidx
 * Return Value        	 -0 - Succes, -1 - Failure
 * Note                 		-  None
 */

void kSt_davinci_set_dma_src_index(int lch, u16 src_bidx, u16 src_cidx)
{
	edma_set_src_index((unsigned) lch, (s16) src_bidx, (s16) src_cidx);

}

/*
 * Function            		- kSt_davinci_set_dma_dest_index
 * Functionality      		- This function acts as an interface for the set dest index API
 * Input Params   	 	- lch, dest_bidx, dest_cidx
 * Return Value        	 -0 - Succes, -1 - Failure
 * Note                 		-  None
 */

void kSt_davinci_set_dma_dest_index(int lch, u16 dest_bidx, u16 dest_cidx)
{
	edma_set_dest_index((unsigned) lch, (s16) dest_bidx, (s16) dest_cidx);
}

/*
 * Function            		- kSt_davinci_set_dma_transfer_params
 * Functionality      		- This function acts as an interface for the set dma transfer params API
 * Input Params   	 	- lch, acnt, bcnt, ccnt, bcntrld, sync_mode
 * Return Value        	 -0 - Succes, -1 - Failure
 * Note                 		-  None
 */

void kSt_davinci_set_dma_transfer_params(int lch, u16 acnt, u16 bcnt, u16 ccnt,
				    u16 bcntrld, enum sync_dimension sync_mode)
{

	edma_set_transfer_params((unsigned) lch,acnt, bcnt, ccnt, bcntrld, sync_mode);

}

/*
 * Function            		- kSt_davinci_get_dma_params
 * Functionality      		- This function acts as an interface for the get dma  params API
 * Input Params   	 	- lch, param_set
 * Return Value        	 -0 - Succes, -1 - Failure
 * Note                 		-  None
 */

void kSt_davinci_get_dma_params(int lch, st_edma_param_set *param_set)
{
	edma_read_slot((unsigned) lch, param_set);

}

/*
 * Function            		- kSt_davinci_set_dma_params
 * Functionality      		- This function acts as an interface for the set dma  params API
 * Input Params   	 	- lch, param_set
 * Return Value        	 -0 - Succes, -1 - Failure
 * Note                 		-  None
 */

void kSt_davinci_set_dma_params(int lch, st_edma_param_set *param_set)
{

	edma_write_slot((unsigned) lch, param_set);

}

/*
 * Function            		- kSt_davinci_start_dma
 * Functionality      		- This function acts as an interface for the start dma API
 * Input Params   	 	- lch
 * Return Value        	 -Return value from start dma API
 * Note                 		-  None
 */

int kSt_davinci_start_dma(int lch)
{
	return edma_start((unsigned)lch);
}

/*
 * Function            		- kSt_davinci_stop_dma
 * Functionality      		- This function acts as an interface for the stop dma API
 * Input Params   	 	- lch
 * Return Value        	 -Return value from stop dma API
 * Note                 		-  None
 */

void kSt_davinci_stop_dma(int lch)
{
	edma_stop((unsigned) lch);
}

/*
 * Function            		- kSt_davinci_free_dma
 * Functionality      		- This function acts as an interface for the free dma API
 * Input Params   	 	- lch
 * Return Value        	 -Return value from free dma API
 * Note                 		-  None
 */

void kSt_davinci_free_dma(int lch)
{
	edma_free_channel((unsigned) lch);
}

/*
 * Function            		- kSt_davinci_dma_link_lch
 * Functionality      		- This function acts as an interface for the link dma API
 * Input Params   	 	- lch_head, lch_queue
 * Return Value        	 -Return value from link dma API
 * Note                 		-  None
 */

void kSt_davinci_dma_link_lch(int lch_head, int lch_queue)
{
	edma_link((unsigned) lch_head, (unsigned) lch_queue);
}







