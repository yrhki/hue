#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

int set_interface_attribs(int fd) {
	struct termios tty;
	if (tcgetattr(fd, &tty) < 0) {
		printf("Error from tcgetaatr: %s\n", strerror(errno));	
		return -1;
	}

	cfsetspeed(&tty, (speed_t)B9600);


	tty.c_cflag |= (CLOCAL | CREAD); /* ignore modem controls */
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;       /* 8-bit characters */
	tty.c_cflag &= ~PARENB;   /* no parity bit */
	tty.c_cflag &= ~CSTOPB;   /* only ned 1 stop bit */
	tty.c_cflag &= ~CRTSCTS;  /* no hardware flowcontrol */

	/* setup for non-canonical mode */
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	tty.c_iflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	tty.c_oflag &= ~OPOST;

	/* fetch bytes as they become available */
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 1;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		printf("Error from tcetatr: %s\n", strerror(errno));	
		return -1;
	}
	return 0;	
}

int open_serial(char* portname) {
	int fd = open(portname, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0) {
		printf("Error opening serial %s: %s\n", portname, strerror(errno));
		return -1;
	}
	return fd;
}

