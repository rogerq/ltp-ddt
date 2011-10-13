/*Application for v4l2 display*/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH_NTSC	720
#define HEIGHT_NTSC	480
#define WIDTH_PAL	720
#define HEIGHT_PAL	576
#define NUM_OF_BUFFERS 	4
#define BYTESPERPIXEL	2


int main(int argc, char* argv[]) {
	int fd;
	int i;
	int output_type = 0;
	char *buffer[4];
	int buff_index;
	int n=1;
	int width = 0, height = 0;
	struct v4l2_output output;
	struct v4l2_format format;
	struct v4l2_requestbuffers reqbuff;
	struct v4l2_buffer querybuf, qbuf;
	
	printf("Entering the application\n\n");
	fd = open("/dev/video5",O_RDWR);
	if(fd < 0) {
		perror("OPEN:");
		printf("failed to open /dev/video5 device\n");
		exit(0);
	}
	printf("/dev/video5 opened\n");
	while(1) {
		output.index = 0;
		output.capabilities = V4L2_OUT_CAP_STD;
		if(ioctl(fd, VIDIOC_ENUMOUTPUT, &output)) {
			printf("errno:%d\n",errno);
			perror("VIDIOC_ENUMOUTPUT:");
			exit(0);
		}
//		if(strcmp(output.name, "Composite") == 0) {
		if(output.name == (__u8*)"Composite"){	
			output_type = output.type;
			printf("1\n");
			break;
		}
		output.index++;
	}
	printf("Output selected is %s\n\n",output.name);
	
	if(output.name == (__u8*)"Composite") {
		
		printf("Setting output\n");
		output_type = 0; //V4L2_OUTPUT_TYPE_ANALOG;
		if(ioctl(fd, VIDIOC_S_OUTPUT, &output_type)) {
			printf("Error setting the output\n");
			exit(0);
		}
		else {
			printf("Output selected is %s\n",output.name);
		}
		printf("Setting the format\n");
		format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
		format.fmt.pix.field = V4L2_FIELD_INTERLACED;
		format.fmt.pix.width = WIDTH_NTSC;
		format.fmt.pix.height = HEIGHT_NTSC;
		format.fmt.pix.bytesperline = width;
		format.fmt.pix.sizeimage = width * height * BYTESPERPIXEL;
		
		if(ioctl(fd, VIDIOC_S_FMT, &format ) < 0) {
			perror("Failed to set the format");
			exit(0);
		}
		printf("Format SET SUCCESFULLY\n\n");
			
		format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		if(ioctl(fd, VIDIOC_G_FMT, &format) < 0) {
			printf("Error getting format");
			exit(0);
		}
		printf(" pitch  = %d\n",format.fmt.pix.bytesperline);
		printf("Requesting for buffers\n");
		reqbuff.count = NUM_OF_BUFFERS;
		reqbuff.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		reqbuff.memory = V4L2_MEMORY_MMAP;
		
		if(ioctl(fd, VIDIOC_REQBUFS, &reqbuff)) {
			printf("Failed to allocate buffers\n");
			exit(0);
		}
		
		printf("Querying buffers\n\n");
		/*Queuing buffer and memory mapping*/
		for(buff_index = 0; buff_index < NUM_OF_BUFFERS; buff_index++) {
			querybuf.index = buff_index;
			querybuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
			querybuf.memory = V4L2_MEMORY_MMAP;
			
			if(ioctl(fd, VIDIOC_QUERYBUF, &querybuf)) {
				printf("Failed to query the buffers\n");
				exit(0);
			}
			buffer[buff_index] = (char*) mmap(0, querybuf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, querybuf.m.offset);
			if ((void*)buffer[buff_index] == (void*) -1) {
				perror("MMAP:");
				exit(0);
			}
		}
		printf("Pitch : %d\n",format.fmt.pix.bytesperline);
		printf("Buffers queried and memory mapped\n\n");
		if(n == 1) {
			width = WIDTH_NTSC;
			height = HEIGHT_NTSC;
		}
		else if(n == 2) {
			width = WIDTH_PAL;
			height = HEIGHT_PAL;
		}
		
		printf("Filling buffer 1\n\n");
		/*Filling buffer 1 with index 0*/	
		/*for(i = 0; i < height;) {
		memset(*buffer[0] + format.fmt.pix.bytesperline * i, 0xff, format.fmt.pix.bytesperline);
		}*/
		int k;
		for(k=0;k<4;k++){
			for(i = 0; i < 691200; i++) {
				if(i<345600)
					*(buffer[k]+i) = 0x8c;
				else
					*(buffer[k]+i) = 0x4c;
			}
		}
		printf("Queueing buffers\n\n");
		for(buff_index = 0; buff_index < NUM_OF_BUFFERS; buff_index++) {
			qbuf.index = buff_index;
			qbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
			qbuf.memory = V4L2_MEMORY_MMAP;
			if(ioctl(fd, VIDIOC_QBUF, &qbuf)) {
				printf("Error queueing the buffer %d\n",buff_index);
				printf("errno:%d\n",errno);
				exit(0);
			}
		}
		printf("streaming buffers\n\n");
		if(ioctl(fd, VIDIOC_STREAMON, &reqbuff.type)) {
			printf("Streamon failed with error\n");
			printf("Errno : %d\n",errno);
			exit(0);
		}
		else {
			printf("video streaming  started\n");
		}
		printf("buffer index : %d\n",buff_index);
		for(i = 0; i < 300; i++) {
			qbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
			qbuf.memory = V4L2_MEMORY_MMAP;
			if(ioctl(fd, VIDIOC_DQBUF, &qbuf)) {
				printf("Errno : %d\n",errno);
				printf("Dequeuing of buffer failed\n");
				exit(0);
			}
			printf("DQ buffer : %d\n\n",qbuf.index);
			
			qbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
			qbuf.memory = V4L2_MEMORY_MMAP;
			if(ioctl(fd, VIDIOC_QBUF, &qbuf)) {
				printf("Queueing of buffer failed\n");
				printf("Errno : %d\n",errno);
				exit(0);
			}
			printf("Q buffer : %d\n",qbuf.index);
			//		getchar();
		}
		
		printf("Streaming off\n\n");
		if(ioctl(fd, VIDIOC_STREAMOFF, &reqbuff.type)) {
			printf("Streaming of failed\n");
			printf("Errno : %d\n",errno);
			exit(0);
		}
		printf("Unmapping the buffers\n\n");
		for(buff_index = 0; buff_index < 4; buff_index++) {
			if(munmap(buffer[buff_index], querybuf.length)) {
				printf("Errno : %d\n",errno);
				printf("Unmapping of buffers failed\n");
				exit(0);
			}
		}
	}	
	return 0;
	}
	
