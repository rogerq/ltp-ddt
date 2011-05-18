
#include <linux/module.h>

#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/watchdog.h>
#include <linux/init.h>
#include <linux/bitops.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/clk.h>

#include <asm/segment.h>
static struct miscdevice edma_dev;

/*Testcode related header files */
#include "kSt_edma_parser.h"
#define EDMA_MODE 0
#define LINK_MODE 1
#define CHAIN_MODE 2
int sync_mode = 0;
int edma_mode = 0;

/*
 * Function            		- kSt_callback1
 * Functionality      		- This function is a completion handler registered with EDMA driver
 * 					 	while requesting the first Master channel
 * Input Params   	 	-  lch, ch_status, data
 * Return Value        	 - None
 * Note                 		-  None
 */ 
void kSt_callback1(unsigned lch, u16 ch_status, void *data) 
{
	switch (ch_status) {
	case ST_DMA_COMPLETE:
		irqraised1 = 1;
		break;
	case ST_DMA_EVT_MISS_ERROR:
		irqraised1 = -1;
		TEST_PRINT_ERR("From Callback 1: DMA_EVT_MISS_ERROR occured "
				  "on Channel %d", lch);
		break;
	case ST_QDMA_EVT_MISS_ERROR:
		irqraised1 = -2;
		TEST_PRINT_ERR("From Callback 1: QDMA_EVT_MISS_ERROR occured "
				  "on Channel %d", lch);
		break;
	default:
		break;
	}
}


/*
 * Function            		- kSt_callback2
 * Functionality      		- This function is a completion handler registered with EDMA driver
 * 					 	while requesting the secone Master channel
 * Input Params   	 	-  lch, ch_status, data
 * Return Value        	 - None
 * Note                 		-  None
 */ 
void kSt_callback2(unsigned lch, u16 ch_status, void *data) 
{
	switch (ch_status) {
	case ST_DMA_COMPLETE:
		irqraised2 = 1;
		break;
	case ST_DMA_EVT_MISS_ERROR:
		irqraised2 = -1;
		TEST_PRINT_ERR("From Callback 2: DMA_EVT_MISS_ERROR occured "
				  "on Channel %d", lch);
		break;
	case ST_QDMA_EVT_MISS_ERROR:
		irqraised2 = -2;
		TEST_PRINT_ERR("From Callback 2: QDMA_EVT_MISS_ERROR occured "
				  "on Channel %d", lch);
		break;
	default:
		break;
	}
}
static int edma_open(struct inode *inode, struct file *filp) 
{
	if (test_and_set_bit(EDMA_IN_USE, &edma_status))
		return -EBUSY;
	return nonseekable_open(inode, filp);
}
static int edma_close(struct inode *inode, struct file *filp) 
{
	clear_bit(EDMA_IN_USE, &edma_status);
	return 0;
}


/*
 * Function             	- st_print_edma_test_params
 * Functionality        	- This function prints the test option values
 * Input Params         	- Test params structure
 * Return Value         	- None
 * Note                 		- None
 */ 
void kSt_print_edma_test_params(struct kSt_edma_info *testoptions) 
{
	TEST_PRINT_TST_START(testoptions->testcaseid);
	TEST_PRINT_TRC("******** edma Testcase  parameters  ******** ");
	TEST_PRINT_TRC("Device : %s", testoptions->device);
	TEST_PRINT_TRC("acnt : %d", testoptions->acnt);
	TEST_PRINT_TRC("bcnt : %d", testoptions->bcnt);
	TEST_PRINT_TRC("ccnt : %d", testoptions->ccnt);
	TEST_PRINT_TRC("numtcs : %d",
			 testoptions->numtcs);
	switch (testoptions->ioctl) {
	case EDMA_ASYNC:
		TEST_PRINT_TRC("Transfer Mode : EDMA_ASYNC");
		break;
	case EDMA_ABSYNC:
		TEST_PRINT_TRC("Transfer Mode : EDMA_ABSYNC");
		break;
	case EDMA_LINK_ASYNC:
		TEST_PRINT_TRC("Transfer Mode : EDMA_LINK_ASYNC");
		break;
	case EDMA_LINK_ABSYNC:
		TEST_PRINT_TRC("Transfer Mode : EDMA_LINK_ABSYNC");
		break;
	case EDMA_CHAIN_ASYNC:
		TEST_PRINT_TRC("Transfer Mode : EDMA_CHAIN_ASYNC");
		break;
	case EDMA_CHAIN_ABSYNC:
		TEST_PRINT_TRC("Transfer Mode : EDMA_CHAIN_ABSYNC");
		break;
	}
	TEST_PRINT_TRC((" ************* End of Test params ************* "));
}

/*
 * Function             	- edma_alloc_buffer
 * Functionality        	- This function allocates the kernel buffer 
 *					   with size MAX_DMA_TRANSFER_IN_BYTES
 * Input Params         	- None
 * Return Value         	- 0 - Succes, -1 - Failure
 * Note                 		- None
 */ 

static int edma_alloc_buffer( void )
{
	int result = 0;
	buffer_size = MAX_DMA_TRANSFER_IN_BYTES;
		/* allocate consistent memory for DMA
		   dmaphyssrc1(handle)= device viewed address.
		   dmabufsrc1 = CPU-viewed address */ 
	dmabufsrc1 =dma_alloc_coherent(NULL, buffer_size, &dmaphyssrc1,0);
	if (!dmabufsrc1) {
		TEST_PRINT_ERR("dma_alloc_coherent failed for dmaphyssrc1");
		result = -ENOMEM;
		goto end;
	}
	dmabufdest1 = dma_alloc_coherent(NULL, buffer_size, &dmaphysdest1,0);
	if (!dmabufdest1) {
		TEST_PRINT_ERR("dma_alloc_coherent failed for dmaphysdest1");
		result = -ENOMEM;
		goto free_src1;
	}
	dmabufsrc2 = dma_alloc_coherent(NULL, buffer_size, &dmaphyssrc2,0);
	if (!dmabufsrc2) {
		TEST_PRINT_ERR("dma_alloc_coherent failed for dmaphyssrc2");
		result = -ENOMEM;
		goto free_dest1;
	}
	dmabufdest2 = dma_alloc_coherent(NULL, buffer_size, &dmaphysdest2,0);
	if (!dmabufdest2) {
		TEST_PRINT_ERR("dma_alloc_coherent failed for dmaphysdest2");
		result = -ENOMEM;
		goto free_src2;
	}
	return result;
	
free_src2:	
	dma_free_coherent(NULL, buffer_size, dmabufsrc2,dmaphyssrc2);

free_dest1:
	dma_free_coherent(NULL, buffer_size, dmabufdest1,dmaphysdest1);

free_src1:
	dma_free_coherent(NULL, buffer_size, dmabufsrc1,dmaphyssrc1);
end:
	return result;

}

/*
 * Function             	- edma_free_buffer
 * Functionality        	- This function frees the kernel buffer 
 * Input Params         	- None
 * Return Value         	- 0 - Succes, -1 - Failure
 * Note                 		- None
 */ 
static int edma_free_buffer(void)
{
	if (dmabufsrc1)
		dma_free_coherent(NULL, buffer_size, dmabufsrc1,dmaphyssrc1);
	if (dmabufdest1)
		dma_free_coherent(NULL, buffer_size, dmabufdest1,dmaphysdest1);
	if (dmabufsrc2)
		dma_free_coherent(NULL, buffer_size, dmabufsrc2,dmaphyssrc2);
	if (dmabufdest2)
		dma_free_coherent(NULL, buffer_size, dmabufdest2,dmaphysdest2);
	return 0;
}

/*
 * Function             	- edma_test
 * Functionality        	- This function calls the specific testcase
 * 					   for various transfer modes 
 * Input Params         	- Test params structure
 * Return Value         	- 0 - Succes, -1 - Failure
 * Note                 		- None
 */  
static int edma_test(struct kSt_edma_info kSt_edma_info)
{
	int result = SUCCESS;
	int j = 0;
	kSt_print_edma_test_params(&kSt_edma_info);
	if(kSt_edma_info.perf_flag)
		performance = 1;
	for (j = 0; j < kSt_edma_info.numtcs; j++) {
		switch (edma_mode) {
		case EDMA_MODE:
			result = kSt_edma_memtomemcpytest(kSt_edma_info.acnt,
							   kSt_edma_info.bcnt,
							   kSt_edma_info.ccnt,
							   sync_mode, j);
			break;
		case LINK_MODE:
			result = kSt_edma_memtomemcpytest_link(kSt_edma_info.acnt,
								kSt_edma_info.bcnt,
								kSt_edma_info.ccnt,
								sync_mode, j);
			break;
		case CHAIN_MODE:
			result = kSt_edma_memtomemcpytest_chain(kSt_edma_info.acnt,
								 kSt_edma_info.bcnt,
								 kSt_edma_info.ccnt,
								 sync_mode, j);
			break;
		}
		

		}
	TEST_PRINT_TST_RESULT(result,kSt_edma_info.testcaseid);
	TEST_PRINT_TST_END(kSt_edma_info.testcaseid);
	return result;
}

/*
 * Function             	- edma_ioctl
 * Functionality        	- Ioct call
 * Input Params         	- Test params structure, ioctl cmd, file pointer
 * Return Value         	- 0 - Succes, -1 - Failure
 * Note                 		- None
 */  
static long edma_ioctl(struct file* file, unsigned int cmd,
			 unsigned long arg) 
{
	int ret = -ENOTTY;
	switch (cmd) {
	case EDMA_ASYNC:
		sync_mode = ASYNC;
		edma_mode = EDMA_MODE;
		ret = copy_from_user(&kSt_edma_info, (struct kSt_edma_info *)arg,
				   sizeof(kSt_edma_info)) ? -EFAULT : 0;
		ret = edma_test(kSt_edma_info);
		break;
	case EDMA_ABSYNC:
		sync_mode = ABSYNC;
		edma_mode = EDMA_MODE;
		ret = copy_from_user(&kSt_edma_info, (struct kSt_edma_info *)arg,
				   sizeof(kSt_edma_info)) ? -EFAULT : 0;
		ret = edma_test(kSt_edma_info);
		break;
	case EDMA_LINK_ASYNC:
		sync_mode = ASYNC;
		edma_mode = LINK_MODE;
		ret = copy_from_user(&kSt_edma_info, (struct kSt_edma_info *)arg,
				   sizeof(kSt_edma_info)) ? -EFAULT : 0;
		ret = edma_test(kSt_edma_info);

		break;
	case EDMA_LINK_ABSYNC:
		sync_mode = ABSYNC;
		edma_mode = LINK_MODE;
		ret = copy_from_user(&kSt_edma_info, (struct kSt_edma_info *)arg,
				   sizeof(kSt_edma_info)) ? -EFAULT : 0;
		ret = edma_test(kSt_edma_info);

		break;
	case EDMA_CHAIN_ASYNC:
		sync_mode = ASYNC;
		edma_mode = CHAIN_MODE;
		ret = copy_from_user(&kSt_edma_info, (struct kSt_edma_info *)arg,
				   sizeof(kSt_edma_info)) ? -EFAULT : 0;
		ret = edma_test(kSt_edma_info);

		break;
	case EDMA_CHAIN_ABSYNC:
		edma_mode = CHAIN_MODE;
		sync_mode = ABSYNC;
		ret = copy_from_user(&kSt_edma_info, (struct kSt_edma_info *)arg,
				   sizeof(kSt_edma_info)) ? -EFAULT : 0;
		ret = edma_test(kSt_edma_info);
		break;
	}
	return ret;
}


static const struct file_operations edma_fops = { 
	.owner = THIS_MODULE, 
	.unlocked_ioctl = edma_ioctl,
	.open = edma_open, 
	.release = edma_close, 
};


/*
 * Function            		- kSt_edma_test_init
 * Functionality      		- This function is a init function registered with module_init
 * Input Params   	 	-  None
 * Return Value        	 - 0 - Succes, -1 - Failure
 * Note                 		-  None
 */ 
static int __init kSt_edma_test_init(void) 
{
	int result = 0;
	edma_dev.minor = EDMA_MINOR;
	edma_dev.name = "edma";
	edma_dev.fops = &edma_fops;
	result = edma_alloc_buffer();
	if(result == FAILURE) {
		TEST_PRINT_ERR("Memory allocation failed");
		goto ret;
	}
	result = misc_register(&edma_dev);
	if (result) {
		TEST_PRINT_ERR("Failed to register the edma device");
		
	}
ret:
	return result;

}


/*
 * Function            		- kSt_edma_test_exit
 * Functionality      		- This function is a exit function registered with module_exit
 * Input Params   	 	-  None
 * Return Value        	 -None
 * Note                 		-  None
 */ 
void kSt_edma_test_exit(void) 
{
	
	misc_deregister(&edma_dev);
	edma_free_buffer();
} 

module_init(kSt_edma_test_init);
module_exit(kSt_edma_test_exit);
MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");

