CFLAGS=-Wall -Werror -Wextra -pedantic -O0 -ggdb
CS=src/main.c src/serial.c src/baudrate.c src/hue.c
HS=src/serial.h src/baudrate.h src/hue.h

hue: $(CS) $(HS)
	$(CC) $(CFLAGS) -o hue $(CS)

