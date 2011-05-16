/*
 * st_fbdev_interface.c
 *
 * This file contains the interface specific funtions
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *	Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 *
 *	Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in the
 *	documentation and/or other materials provided with the
 *	distribution.
 *
 *	Neither the name of Texas Instruments Incorporated nor the names of
 *	its contributors may be used to endorse or promote products derived
 *	from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/* header file */
#include "st_fbdev_common.h"
#include "st_fbdev_interface.h"

static inline int config_dss_sysfs(const char *file, char *val)
{
	char f_name[100];

	sprintf(f_name, "/sys/devices/platform/omapdss/%s", file);
	return write_to_sysfs(f_name, val);
}
/*
 * Platform specific help/usage (like custom IOCTL, features)
 */
static void st_display_fbdev_display_custom_help(void)
{
	printf("-ioctl_no	IOCTL number\n"
			"\t\t\tPossible values - 7:fbioqueryplane, "
			"8:fbioqueryMem, 9:fbioGetUpdateMode\n");
}

/*
 * Wait for Vsync (required for panning/double buffering)
 */
static int st_fbdev_waitforsync_interface(int fd)
{
	int ret;

	ret = ioctl(fd, OMAPFB_WAITFORVSYNC, 0);
	if (SUCCESS != ret)
		perror ("OMAPFB_WAITFORVSYNC: ");

	return ret;
}

static void get_resolution_from_sysfs(char *disp, unsigned int *w,
		unsigned int *h)
{
	int ret;
	char file_path[100], val[100], *ptr;

	sprintf(file_path, "/sys/devices/platform/omapdss/%s/timings", disp);

	ret = read_from_sysfs(file_path, val);
	if (ret < 0) {
		TEST_PRINT_ERR("Error in opening %s\n", disp);
		return;
	}

	ptr = val;
	while (*ptr != ',')
		ptr++;
	*w = strtoul(ptr+1, &ptr, 0);

	while (*ptr != ',')
		ptr++;
	*h = strtoul(ptr+1, &ptr, 0);
}

static int configure_fb(char *dev, struct fb_var_screeninfo *var)
{
	int ret, fd;

	ret = open(dev, O_RDWR);
	if (ret == -1)
		return ret;
	fd = ret;

	ret = st_fbdev_put_vscreeninfo_interface(fd, var);

	ret |= close(fd);

	return ret;
}
/* Set the requested display output */
static int st_fbdev_set_output_interface(struct fbdev_display_testparams
		*options)
{
	unsigned int width, height;

	/*
	 * In case of OMAP35x we are supporting 3 outputs
	 *	0 = Switch to LCD
	 *	1 = Switch to DVI
	 *	2 = Switch to TV
	 */
	switch (options->output) {
	case 0:
		TEST_PRINT_TRC("Switching output to LCD output\n");
		/* Disable overlays and displays and break the link */
		config_dss_sysfs("overlay0/enabled", "0");
		config_dss_sysfs("overlay1/enabled", "0");
		config_dss_sysfs("overlay2/enabled", "0");
		config_dss_sysfs("overlay0/manager", "\n");
		config_dss_sysfs("overlay1/manager", "\n");
		config_dss_sysfs("overlay2/manager", "\n");
		config_dss_sysfs("display0/enabled", "0");
		config_dss_sysfs("display1/enabled", "0");
		config_dss_sysfs("display2/enabled", "0");
		/* Set the configuration for Fbdev: currently only fb0 is being
		 * considered */
		get_resolution_from_sysfs("display0", &width, &height);
		if (options->var.rotate == 1 || options->var.rotate == 3) {
			options->var.xres = height;
			options->var.yres = width;
		} else {
			options->var.xres = width;
			options->var.yres = height;
		}
		configure_fb(options->device_name, &options->var);

		/* Set the links and enable overlays: Currentl only GFX is
		 * enabled */
		config_dss_sysfs("overlay0/manager", "lcd");
		config_dss_sysfs("overlay1/manager", "lcd");
		config_dss_sysfs("overlay2/manager", "lcd");
		config_dss_sysfs("manager0/display", "lcd");
		config_dss_sysfs("overlay0/enabled", "1");
		config_dss_sysfs("display0/enabled", "1");
		/* Set the configuration for Fbdev: currently only fb0 is being
		 * considered */
		configure_fb(options->device_name, &options->var);
		break;
	case 1:
		TEST_PRINT_TRC("Switching output to DVI output\n");
		/* Disable overlays and displays and break the link */
		config_dss_sysfs("overlay0/enabled", "0");
		config_dss_sysfs("overlay1/enabled", "0");
		config_dss_sysfs("overlay2/enabled", "0");
		config_dss_sysfs("overlay0/manager", "\n");
		config_dss_sysfs("overlay1/manager", "\n");
		config_dss_sysfs("overlay2/manager", "\n");
		config_dss_sysfs("display0/enabled", "0");
		config_dss_sysfs("display1/enabled", "0");
		config_dss_sysfs("display2/enabled", "0");
		/* Set the configuration for Fbdev: currently only fb0 is being
		 * considered */
		get_resolution_from_sysfs("display2", &width, &height);
		if (options->var.rotate == 1 || options->var.rotate == 3) {
			options->var.xres = height;
			options->var.yres = width;
		} else {
			options->var.xres = width;
			options->var.yres = height;
		}
		configure_fb(options->device_name, &options->var);
		/* Set the links and enable overlays: Currentl only GFX is
		 * enabled */
		config_dss_sysfs("manager0/display", "dvi");
		config_dss_sysfs("overlay0/manager", "lcd");
		config_dss_sysfs("overlay1/manager", "lcd");
		config_dss_sysfs("overlay2/manager", "lcd");
		config_dss_sysfs("overlay0/enabled", "1");
		config_dss_sysfs("display2/enabled", "1");
		/* Set the configuration for Fbdev: currently only fb0 is being
		 * considered */
		configure_fb(options->device_name, &options->var);
		break;
	case 2:
		TEST_PRINT_TRC("Switching output to TV output\n");
		/* Disable overlays and displays and break the link */
		config_dss_sysfs("overlay0/enabled", "0");
		config_dss_sysfs("overlay1/enabled", "0");
		config_dss_sysfs("overlay2/enabled", "0");
		config_dss_sysfs("overlay0/manager", "\n");
		config_dss_sysfs("overlay1/manager", "\n");
		config_dss_sysfs("overlay2/manager", "\n");
		config_dss_sysfs("display0/enabled", "0");
		config_dss_sysfs("display1/enabled", "0");
		config_dss_sysfs("display2/enabled", "0");
		/* Set the configuration for Fbdev: currently only fb0 is being
		 * considered */
		get_resolution_from_sysfs("display1", &width, &height);
		if (options->var.rotate == 1 || options->var.rotate == 3) {
			options->var.xres = height;
			options->var.yres = width;
		} else {
			options->var.xres = width;
			options->var.yres = height;
		}
		configure_fb(options->device_name, &options->var);

		config_dss_sysfs("overlay0/manager", "tv");
		config_dss_sysfs("overlay1/manager", "tv");
		config_dss_sysfs("overlay2/manager", "tv");
		config_dss_sysfs("overlay0/enabled", "1");
		config_dss_sysfs("display1/enabled", "1");
		/* Set the configuration for Fbdev: currently only fb0 is being
		 * considered */
		configure_fb(options->device_name, &options->var);
		break;
	default:
		TEST_PRINT_ERR("Wrong output device\n");
		return -EINVAL;
	}

	return 0;
}

/* Platform init */
void st_fbdev_init_platform_api(struct fbdev_func_table *func)
{
	if (!func)
		return;

	func->help = st_display_fbdev_display_custom_help;
	func->wait_for_vsync = st_fbdev_waitforsync_interface;
	func->set_output = st_fbdev_set_output_interface;
}
