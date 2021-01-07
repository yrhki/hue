#include <stdint.h>
#include <unistd.h>
#include "hue.h"

#include <stdio.h>

int read_status(int fd) {
	uint8_t resp[1];
	read(fd, resp, sizeof(resp));
	return resp[0];
}

int init(int fd) {
	uint8_t buffer[COMMAND_LENGTH] = {00};
	buffer[0] = 75;
	write(fd, buffer, sizeof(buffer));
	sleep(1);
	return read_status(fd);
}


Strip strip_info(int fd, int channel) {
	Strip r = {0};
	uint8_t buf[2] = {141, channel};
	write(fd, buf, sizeof(buf));
	uint8_t resp[5];
	sleep(1);
	read(fd, resp, sizeof(resp));
	r.length = resp[sizeof(resp)-1];
	return r;
}

Command set_strip(int fd, int channel, int mode, int direction, int option, int group, int speed, Color* colors, int color_len) {
	Command com = {0};
	uint8_t len;
	Strip strip = strip_info(fd, channel);
	len = strip.length-1;

	com.arr[0] = 75;
	com.arr[1] = channel;
	com.arr[2] = mode;
	com.arr[3] = direction << 4 | option << 3 | len;

	for (int i = 0; i < color_len; ++i) {
		com.arr[4] = i << 5 | group << 3 | speed;
		for (int c = 5; c  < COMMAND_LENGTH; c+=3) {
			com.arr[c] = colors[i].Green;	
			com.arr[c+1] = colors[i].Red;	
			com.arr[c+2] = colors[i].Blue;	
		}
		
		write(fd, com.arr, COMMAND_LENGTH);
		sleep(1);
		if (read_status(fd) != 1) {
			fprintf(stderr, "Error occured: R:%d G:%d B:%d\n", colors[i].Red, colors[i].Green, colors[i].Blue);	
		} 
		
	}
	return com;
}
