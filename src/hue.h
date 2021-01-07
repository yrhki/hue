//typedef struct strip Strip;
#include <stdint.h>
#define COMMAND_LENGTH 125

typedef enum MODE {
	FIXED = 0,
	FADING,
	SPECTRUM,
	MARQUEE,
	COVER_MARQUEE,
	ALTERNATING,
	PULSE,
	BREATHING,
	ALERT,
	CANDLE,
	WINGS = 12,
	WAVE = 13
} MODE;

typedef struct Color {
	uint8_t Green;
	uint8_t Red;
	uint8_t Blue;
} Color;

typedef struct Strip {
	uint8_t length;
} Strip;
typedef struct Command {
	uint8_t arr[COMMAND_LENGTH];
} Command;

Strip strip_info(int fd,int channel);
int init(int fd);
Command set_strip(int fd, int channel, int mode, int direction, int option, int group, int speed, Color* colors, int color_len);
