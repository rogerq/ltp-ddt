#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38)
#include <linux/videodev.h>
#endif
#include <linux/videodev2.h>
#include <media/davinci/vpfe_capture.h>
#include <linux/media.h>
#include <linux/v4l2-subdev.h>

#ifndef COMMON_H
#define COMMON_H

/* mc related */

/* Media entity names */
#define E_VIDEO_CCDC_OUT_NAME	"DAVINCI VIDEO CCDC output"
#define E_VIDEO_PRV_OUT_NAME	"DAVINCI VIDEO PRV output"
#define E_VIDEO_PRV_IN_NAME		"DAVINCI VIDEO PRV input"
#define E_VIDEO_RSZ_OUT_NAME	"DAVINCI VIDEO RSZ output"
#define E_VIDEO_RSZ_IN_NAME		"DAVINCI VIDEO RSZ input"
#define E_TVP514X_NAME		"tvp514x"
#define E_TVP7002_NAME		"tvp7002"
#define E_MT9T111_NAME		"mt9t111"
#define E_CCDC_NAME		"DAVINCI CCDC"
#define E_PRV_NAME		"DAVINCI PREVIEWER"
#define E_RSZ_NAME		"DAVINCI RESIZER"
#define E_AEW_NAME		"DAVINCI AEW"
#define E_AF_NAME		"DAVINCI AF"
#define PRESET		V4L2_DV_720P60

/* pad id's as enumerated by media device*/
#define P_RSZ_SINK	0 /* sink pad of rsz */
#define P_RSZ_SOURCE	1 /* source pad of rsz */
#define P_PRV_SINK	0
#define P_PRV_SOURCE	1
#define P_RSZ_VID_OUT	0 /* only one pad for video node */
#define P_RSZ_VID_IN	0 /* only one pad for video node */
#define P_PRV_VID_IN	0
#define P_PRV_VID_OUT	0
#define P_TVP514X	0 /* only one pad for decoder */
#define P_TVP7002	0 /* only one pad for decoder */
#define P_MT9P031	0 /* only one pad for sensor */
#define P_CCDC_SINK	0 /* sink pad of ccdc */
#define P_CCDC_SOURCE	1 /* source pad which connects video node */
#define P_VIDEO		0 /* only one input pad for video node */
#define P_AEW		0
#define P_AF		0
#define IMG_WIDTH_PAL_NTSC	720
#define IMG_HEIGHT_NTSC		480
#define IMG_HEIGHT_PAL		576

#define IMG_WIDTH_VGA		640
#define IMG_HEIGHT_VGA		480

#endif

#define ALIGN(x, y)	(((x + (y-1))/y)*y)
#define CLEAR(x) memset (&(x), 0, sizeof (x))

#define NUM_BUFFERS     	3
#define CAPTURE_MAX_BUFFER	NUM_BUFFERS
#define DISPLAY_MAX_BUFFER	NUM_BUFFERS
#define CAPTURE_DEVICE		"/dev/video0"
#define DISPLAY_DEVICE		"/dev/video5"
#define MEDIA_DEVICE		"/dev/media0"
#define MEDIA_NAME		"Media"
#define MAXLOOPCOUNT		1000


/* structure to hold address */
struct buf_info {
	int index;
	unsigned int length;
	char *start;
};

static struct buf_info capture_buffers[CAPTURE_MAX_BUFFER];
static struct buf_info display_buffers[DISPLAY_MAX_BUFFER];

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
	int tvp7002;
	int mt9t111;
	int prv;
	int rsz;
	int hd;
	/* Total number of entities */
	unsigned int num_entities;
};
#define APP_NUM_BUFS	3

/* capture buffer addresses stored here */
#define BYTESPERPIXEL	2

struct media_entity_desc	entity[15];
struct media_links_enum		links;
int				entities_count;

extern void media_device_close(int media_fd);
extern int media_device_open(struct media_dev *media);
extern int enumerate_all_entities(struct media_dev *media);
extern int reset_media_links(struct media_dev *media);
extern int setup_media_links(struct media_dev *media);
extern int enumerate_all_entities_prv(struct media_dev *media);
extern int setup_media_links_rsz(struct media_dev *media);
extern int enumerate_all_entities_rsz(struct media_dev *media);

#define OPTION_TVP514X 1
#define SUCCESS 0
#define TRUE 0
#define FALSE 1
#define OPTION_WIDTH 720
#define OPTION_HEIGHT 480
#define OPTION_HELP

struct mc_capture_testparams {
	int tvp514x;
	int inp_width;
	int inp_height;
};

extern int mc_tvp514_test(struct mc_capture_testparams *testoptions);
extern int mc_tvp7002_test(struct mc_capture_testparams *testoptions);
extern int prv();
extern int mc_tvp514_prv_rzr(struct mc_capture_testparams *testoptions);


