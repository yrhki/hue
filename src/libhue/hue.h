#ifndef HUE_H
#define HUE_H

#include <stdint.h>

#define HUE_PACKET_LENGTH 125
#define HUE_MAX_LEDS 40
#define HUE_LEDS_ARRAY_LENGTH 8

#define HUE_PACKET_FILL_COLOR(hue, color) \
    for (int ci = 0; ci < HUE_MAX_LEDS; ++ci) hue.colors[ci] = color

typedef enum hue_mode {
	HUE_MODE_FIXED = 0,
	HUE_MODE_FADING,
	HUE_MODE_SPECTRUM,
	HUE_MODE_MARQUEE,
	HUE_MODE_COVER_MARQUEE,
	HUE_MODE_ALTERNATING,
	HUE_MODE_PULSE,
	HUE_MODE_BREATHING,
	HUE_MODE_ALERT,
	HUE_MODE_CANDLE,
	HUE_MODE_WINGS = 12,
	HUE_MODE_WAVE,
	HUE_MODE_DIRECT
} hue_mode;

/*! \enum DIRECTION
 *
 *  Detailed description
 */
typedef enum hue_direction {
    HUE_DIRECTION_FORWARD,
    HUE_DIRECTION_BACKWARD
} hue_direction;

/*! \enum SPEED
 *
 *  Detailed description
 */
typedef enum hue_speed {
    HUE_SPEED_1,
    HUE_SPEED_2,
    HUE_SPEED_3,
    HUE_SPEED_4,
    HUE_SPEED_5
} hue_speed;

/*! \enum GROUP
 *
 *  Detailed description
 */
typedef enum hue_group {
    HUE_GROUP_2,
    HUE_GROUP_3,
    HUE_GROUP_5,
    HUE_GROUP_10
} hue_group;

/*! \enum DIRECTION
 *
 *  Detailed description
 */
typedef enum hue_movement {
    HUE_MOVEMENT_STILL,
    HUE_MOVEMENT_MOVING
} hue_movement;

/*! \enum err
 *
 *  ekajek
 */
typedef enum hue_error {
    HUE_ERR_NONE,
    // Read error
    HUE_ERR_READ = -1,
    // Write error
    HUE_ERR_WRITE = -2,
    // Invalid arguments
    HUE_ERR_ARG = -3,
    // Invalid strip info
    HUE_ERR_STRIP = -4,
    // Non ok status returned
    HUE_ERR_STATUS = -5,
    // No leds
    HUE_ERR_LED = -6,
} hue_error;

typedef enum hue_strip_length {
    HUE_STRIP_0_00,
    HUE_STRIP_1_10,
    HUE_STRIP_2_20,
    HUE_STRIP_3_30,
    HUE_STRIP_4_40,
} hue_strip_length;

typedef struct hue_color {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} hue_color;

typedef struct hue_channel {
    int channel;
	hue_strip_length length;
} hue_channel;

typedef struct hue_packet {
    uint8_t channel;
    hue_mode mode;
    hue_direction direction;
    hue_movement movement;
    hue_group group;
    hue_speed speed;
	hue_color colors[HUE_MAX_LEDS];
} hue_packet;

enum hue_error hue_strip_info(int fd, int channel_i, hue_channel *channel);
enum hue_error hue_init(int fd);
int hue_open(char* portname);
enum hue_error hue_set_strip(int fd, int channel, hue_mode mode, hue_direction direction, hue_movement movement, hue_group group, hue_speed speed, hue_color* colors, int color_len);
enum hue_error hue_send(int fd, hue_packet *hue, hue_channel *channel, uint8_t color_index);

hue_packet hue_mode_fixed(int fd, int channel, hue_color color);

#endif /* ifndef HUE_H */
