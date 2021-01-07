#include <stdio.h>
#include <asm-generic/termbits.h>
#include <sys/ioctl.h>

int set_custom_baudrate(int fd) {
    int speed = 256000;
    struct termios2 tio;
    ioctl(fd, TCGETS2, &tio);

	tio.c_cflag = 0;
	tio.c_iflag = 0;
	tio.c_lflag = 0;


    tio.c_cflag &= ~CBAUD;
    tio.c_cflag |= BOTHER;
    tio.c_ispeed = speed;
    tio.c_ospeed = speed;
    int r = ioctl(fd, TCSETS2, &tio);
    if (r == 0) {
        //printf("Changed successfully.\n");
    } else {
        perror("ioctl");
    }
	return 0;
}
