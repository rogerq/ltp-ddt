

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <linux/media.h>
#include <linux/v4l2-mediabus.h>
#include <linux/v4l2-subdev.h>
#include <linux/videodev.h>
#include <linux/videodev2.h>


/* structure used to store information of the buffers */
struct buf_info {
	int index;
	unsigned int length;
	char *start;
};

struct media_dev {
	/* Media device */
	int media_fd;
	/* either tvp5146 or mt9t111 */
	int input_source;
	/* All entities */
	struct media_entity_desc entity[20];
	/* Entities we do care about */
	int video;	/* Streaming entity */
	int ccdc;
	int tvp5146;
	int mt9t111;

	/* Total number of entities */
	unsigned int num_entities;
};

struct capture_dev {
	int capture_fd;

	struct v4l2_format capture_fmt;
	struct v4l2_buffer capture_buf;

	int tvp_input;	/* Only applicable for tvp5146 */

	unsigned int num_bufs;
	/* Width and height for current input */
	unsigned int width;
	unsigned int height;
};

struct display_dev {
	int display_fd;

	struct v4l2_format display_fmt;
	struct v4l2_buffer display_buf;

	unsigned int num_bufs;
	/* Width and height for current input */
	unsigned int width;
	unsigned int height;
};

struct mem_allocator_dev {
        int buffer_fd;

        struct v4l2_format display_fmt;
        struct v4l2_buffer display_buf;

        unsigned int num_bufs;
        /* Width and height for current input */
        unsigned int width;
        unsigned int height;
};


/* Changing the following will result in different number of buffers used */
#if defined (CONFIG_AM3517)
#define CAPTURE_DEVICE		"/dev/video0"
#define DISPLAY_DEVICE		"/dev/video1" 
#define VIDEO2_DEVICE		"/dev/video2"
#elif defined (CONFIG_OMAP3530)
#define CAPTURE_DEVICE		"/dev/video2"
#define DISPLAY_DEVICE		"/dev/video7"
#define VIDEO2_DEVICE		"/dev/video7"
#define MEDIA_DEVICE		"/dev/media0"

/* Media entity names */
#define ENTITY_VIDEO_CCDC_OUT_NAME	"OMAP3 ISP CCDC output"
#define ENTITY_CCDC_NAME		"OMAP3 ISP CCDC"
#define ENTITY_TVP514X_NAME		"tvp514x 3-005c"
#define ENTITY_MT9T111_NAME		"mt9t111 2-003c"
#define TOTAL_MAX_BUFFER        6
#define VIDEO2_NAME		"Mem-Allocator"
#endif

#define NUM_BUFFERS     	3
#define CAPTURE_MAX_BUFFER	NUM_BUFFERS
#define DISPLAY_MAX_BUFFER	NUM_BUFFERS
#define CAPTURE_NAME		"Capture"
#define DISPLAY_NAME		"Display"
#define MEDIA_NAME		"Media"
/* number of frames to be captured and displayed */
#define MAXLOOPCOUNT		1000

#define DEF_PIX_FMT		V4L2_PIX_FMT_UYVY
#define IMG_WIDTH_PAL_NTSC	720
#define IMG_HEIGHT_NTSC		480
#define IMG_HEIGHT_PAL		574

#define IMG_WIDTH_VGA		640
#define IMG_HEIGHT_VGA		480

/* capture_buff_info and display_buff_info stores buffer information of capture
   and display respectively. */
struct buf_info capture_buff_info[CAPTURE_MAX_BUFFER];
struct buf_info display_buff_info[DISPLAY_MAX_BUFFER];
static struct buf_info video2_buff_info[TOTAL_MAX_BUFFER];
/* Function declaration */
extern int  open_video_dev(const char *dev, int *capture_fd);


extern int media_device_open(struct media_dev *media);
extern int enumerate_all_entities(struct media_dev *media);
extern int reset_media_links(struct media_dev *media);
extern int setup_media_links(struct media_dev *media);
extern int enumerate_all_entities_prv(struct media_dev *media);
extern int setup_media_links_rsz(struct media_dev *media);

extern int display_prepare_streaming(struct display_dev *display);
extern int capture_prepare_streaming(struct capture_dev *capture);
extern int get_current_capture_format(int input_src, struct capture_dev *capture);

extern int timeval_subtract(struct timeval *result, struct timeval *x,
                                     struct timeval *y);

extern void media_device_close(int media_fd);
extern int set_subdev_format(struct media_dev *media,struct capture_dev *capture);
extern void close_video_dev(int capture_fd);
extern int capture_display_prepare_buffers(struct mem_allocator_dev *mem_allocator);

