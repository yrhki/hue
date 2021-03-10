#include <stdint.h>
#include <unistd.h>
#include "hue.h"
#include "serial.h"
#include "baudrate.h"

#include <stdio.h>

// #define SLEEPTIME 50000
#define SLEEPTIME 70000
#define CHECKERR(value, ret) if (value < 0) return ret
#define VMINMAX(value, min, max) \
    if (value > max || value < min) return HUE_ERR_ARG


enum hue_error hue_read_status(int fd) {
    uint8_t resp[2];
    int ret = read(fd, resp, sizeof(resp));
    CHECKERR(ret, HUE_ERR_READ);
    if (resp[0] != 1) return HUE_ERR_STATUS;
    return 0;
}

enum hue_error hue_init(int fd) {
    hue_packet hue = {0};
    hue_channel channel = {0};
    channel.length = HUE_STRIP_1_10;
    hue_error err = hue_send(fd, &hue, &channel, 0);
    CHECKERR(err, err);
    return hue_read_status(fd);
}

int hue_open(char* portname) {
    int fd = open_serial(portname);
    if (fd < 0) return -1;
    int ret = set_interface_attribs(fd);
    if (ret < 0) return -2;
    ret = set_custom_baudrate(fd);
    if (ret < 0) return -3;
    return fd;
}

int command_prepare(hue_packet *hue, int channel, hue_mode mode, hue_direction direction, hue_movement movement, hue_group group, hue_speed speed) {
    hue->channel = channel;
    hue->mode = mode;
    hue->direction = direction;
    hue->movement = movement;
    hue->group = group;
    hue->speed = speed;
    return 0;
}

enum hue_error hue_channel_info(int fd, int channel, hue_channel* strip) {
    uint8_t buf[2] = {141, channel};
    int ret = write(fd, buf, sizeof(buf));
    CHECKERR(ret, HUE_ERR_WRITE);

    uint8_t resp[5] = {0};
    usleep(SLEEPTIME);

    ret = read(fd, resp, sizeof(resp));
    CHECKERR(ret, HUE_ERR_READ);

    if (resp[0] != 192) return HUE_ERR_STRIP;

    if (resp[4] == 0) return HUE_ERR_LED;
    #ifdef DEBUG
    printf("channel %d: LEN:%d\n", channel, resp[4]);
    #endif
    strip->length = resp[4];
    return 0;
}

hue_packet hue_mode_fixed(int fd, int channel, hue_color color) {
    hue_channel ch = {0};
    hue_channel_info(fd, channel, &ch);
    hue_packet hue = {0};
    command_prepare(&hue, channel, HUE_MODE_FIXED, 0, 0, 0, 0);
    for (int c = 0; c  < HUE_MAX_LEDS; c++) {
        hue.colors[c] = color;  
    }
    return hue;
}

#ifdef DEBUG
void hue_print(char* name, hue_packet *hue) {
    printf("%s: C:%d M:%d D:%d M:%d G:%d S:%d", name, hue->channel,
            hue->mode, hue->direction, hue->movement, hue->group, hue->speed);
    
    for (int c = 0; c < HUE_MAX_LEDS; c++) {
       printf(" R%03d:G%03d:B%03d", hue->colors[c].red,
               hue->colors[c].green, hue->colors[c].blue);
    }
    printf("\n");
}
#endif

enum hue_error hue_send(int fd, hue_packet *hue, hue_channel *channel, uint8_t index) {
    uint8_t buf[HUE_PACKET_LENGTH] = {0};

    // Safety check
    VMINMAX(hue->direction, HUE_DIRECTION_FORWARD, HUE_DIRECTION_BACKWARD);
    VMINMAX(hue->speed, HUE_SPEED_1, HUE_SPEED_5);
    VMINMAX(hue->group, HUE_GROUP_2, HUE_GROUP_10);
    VMINMAX(hue->movement, HUE_MOVEMENT_STILL, HUE_MOVEMENT_MOVING);
    VMINMAX(hue->mode, HUE_MODE_FIXED, HUE_MODE_DIRECT);
    VMINMAX(channel->length, HUE_STRIP_0_00, HUE_STRIP_4_40);

    buf[0] = 75;
    buf[1] = hue->channel;
    buf[2] = hue->mode;
    buf[3] = hue->direction << 4 | hue->movement << 3 | (channel->length-1);
    buf[4] = index << 5 | hue->group << 3 | hue->speed;

    if (buf[3] > 30) return HUE_ERR_ARG;

    for (int b = 5,c = 0; b < HUE_PACKET_LENGTH; b+=3,c++) {
        buf[b]   = hue->colors[c].green;    
        buf[b+1] = hue->colors[c].red;  
        buf[b+2] = hue->colors[c].blue; 
    }
    
    int ret = write(fd, buf, HUE_PACKET_LENGTH);
    CHECKERR(ret, HUE_ERR_WRITE);
    #ifdef DEBUG
    hue_print("SEND", hue);
    #endif
    usleep(SLEEPTIME);
    return 0;
}

enum hue_error hue_set_strip(int fd, int channel, hue_mode mode, hue_direction direction, hue_movement movement, hue_group group, hue_speed speed, hue_color* colors, int color_len) {
    hue_packet hue = {0};
    hue_channel strip = {0};
    hue_error err;

    err = hue_channel_info(fd, channel, &strip);
    CHECKERR(err, err);

    hue.channel = channel;
    hue.mode = mode;
    hue.direction = direction;
    hue.movement = movement;
    hue.group = group;
    hue.speed = speed;

    for (int i = 0; i < color_len; ++i) {
        HUE_PACKET_FILL_COLOR(hue, colors[i]);
        
        err = hue_send(fd, &hue, &strip, i);
        CHECKERR(err, err);
        err = hue_read_status(fd);
        CHECKERR(err, err);
    }
    return 0;
}
