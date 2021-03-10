#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>

#include "libhue/hue.h"

#define CURRENT_ARG argv[optind]
#define MAXV(value, max, name)                            \
    if (value > max || value < 0) {                       \
        invalid_options = 1;                              \
        fprintf(stderr, "Invalid option %s\n", name);   \
    }

#define MAX_SPEED 4
#define MAX_GROUP 3
#define MAX_CHANNEL 2

int parse_color(char* hexstring, hue_color* color) {
	size_t length = strlen(hexstring);
	if (length == 3 || length == 6) {
		int delim = length / 3;
		char red[3] = {0};
		char green[3] = {0};
		char blue[3] = {0};

		memcpy(red, hexstring, delim * sizeof(char));
		strncpy(green, hexstring + (delim * 1), delim);
		strncpy(blue, hexstring + (delim * 2), delim);
	
		color->red = strtol(red, NULL, 16); 
		color->green = strtol(green, NULL, 16); 
		color->blue = strtol(blue, NULL, 16); 
		return 0;
	} else {
		return -1;
	}
}

int parse_mode(char* input, hue_mode *mode) {
		if (strcmp(input, "fixed") == 0)
            *mode = HUE_MODE_FIXED;
		else if (strcmp(input, "fading") == 0)
			*mode = HUE_MODE_FADING;
		else if (strcmp(input, "spectrum") == 0)
			*mode = HUE_MODE_SPECTRUM;
		else if (strcmp(input, "marquee") == 0)
			*mode = HUE_MODE_MARQUEE;
		else if (strcmp(input, "cmarquee") == 0)
			*mode = HUE_MODE_COVER_MARQUEE;
		else if (strcmp(input, "alternating") == 0)
			*mode = HUE_MODE_ALTERNATING;
		else if (strcmp(input, "pulse") == 0)
			*mode = HUE_MODE_PULSE;
		else if (strcmp(input, "breathing") == 0)
			*mode = HUE_MODE_BREATHING;
		else if (strcmp(input, "alert") == 0)
			*mode = HUE_MODE_ALERT;
		else if (strcmp(input, "candle") == 0)
			*mode = HUE_MODE_CANDLE;
		else if (strcmp(input, "wings") == 0)
			*mode = HUE_MODE_WINGS;
		else if (strcmp(input, "wave") == 0)
			*mode = HUE_MODE_WAVE;
		else if (strcmp(input, "direct") == 0)
			*mode = HUE_MODE_DIRECT;
		else {
            return -1;
		}
        return 0;
}

int get_colors(int argc, char* argv[], hue_color* colors[], int *optind, int min, int max)
{
    assert(optind);
    int color_len = argc - *optind;
    *colors = malloc(sizeof(hue_color) * color_len);
    if (min == 0 && max == 0) return 1;
    if (color_len < min) return -2;
    if (color_len > max) color_len = max;
    hue_color *c = *colors;
    for (int i = 0; i < color_len; ++i) {
        c[i].blue = 2;
        c[i].red = 0;
        c[i].green = 0;
        if (parse_color(argv[*optind], &c[i]) < 0) {
            return -3;
        }
        *optind = *optind + 1;
    }
    return color_len;
}

void print_usage(char* name)
{
    printf("Usage: %s\n"
           "    init                      reset leds\n"
           "    mode                      set led colors\n"
           "        fixed <color>         set static color\n"
           "        fading <colors>       set fading colors 2..8\n"
           "                              options: speed\n"                 
           "        spectrum              set spectrum\n"
           "                              options: speed, reverse\n"                 
           "        marquee <color>       set marquee\n"
           "                              options: speed, group, reverse\n"                 
           "        cmarquee <color>      set cover marquee 2..8\n"
           "                              options: speed, reverse\n"                 
           "        alternating <colors>  set alternating between 2 colors\n"
           "                              options: speed, moving, group, reverse\n"                 
           "        pulse <colors>        set pulse 1..8 colors\n"
           "                              options: speed\n"                 
           "        breathing <colors>    set breathing 1..8 colors\n"
           "                              options: speed\n"                 
           "        alert <colors>        set alert 1..2 colors\n"
           "                              group changes blinking interval\n"                 
           "                              don't use speed\n"                 
           "                              options: group, speed\n"                 
           "        candle <color>        set candle\n"
           "                              options: speed\n"                 
           "        wings <color>         set wings\n"
           "                              options: speed\n"                 
           "        wave <color>          set wave\n"
           "        direct <color>        same as fixed\n"
           "\n", name);
}

int main(int argc, char* argv[]) {
	char* name = argv[0];
	char* portname = "/dev/ttyACM0";
	int EXIT_CODE = 0;

    // Options
    enum hue_direction direction = 0;
    enum hue_speed speed = 0;
    enum hue_group group = 0;
    enum hue_movement movement = 0;
    int invalid_options = 0;

	while (1) {
		static struct option long_options[] =
		{
			{"version", no_argument,		NULL,	0},
			{"help",	no_argument,		NULL,	0},
			{"serial",	required_argument,	NULL,	0},
			{"speed",	required_argument,	NULL,	's'},
			{"group",	required_argument,	NULL,	'g'},
			{"reverse",	no_argument,	    NULL,	'r'},
			{"moving",	no_argument,	    NULL,	'm'},
			
			{0,0,0,0}
		};
		int c, option_index;

		c = getopt_long(argc, argv, "vhs:g:rm", long_options, &option_index);
		if (c == -1) break;
		switch (c) {
            case 'm':
                movement = HUE_MOVEMENT_MOVING;
				break;
            case 'r':
                direction = HUE_DIRECTION_BACKWARD;
				break;
            case 's':
                speed = atoi(optarg);
                MAXV(speed, MAX_SPEED, "speed");
				break;
            case 'g':
                group = atoi(optarg);
                MAXV(group, MAX_GROUP, "group");
				break;
			case 'v':
				break;
			default:
                print_usage(name);
				exit(0);
		}
	}	
    #ifdef DEBUG
    printf("SPEED: %d, GROUP: %d, REVERSE: %d, MOVING: %d\n", speed, group, direction, movement);
    #endif

    if (invalid_options == 1) exit(1);

	if (optind >= argc) {
		fprintf(stderr, "Expected argument after options\n");
		exit(EXIT_FAILURE);
	}

	int fd = hue_open(portname);
	if (fd < 0) return 1;
	char* action = CURRENT_ARG;

	if (strcmp(action, "mode") == 0) {
		optind++;
		if (optind >= argc) {
			fprintf(stderr, "No mode passed\n");
			goto close_error;
		}
        hue_mode mode = 0;

        if (parse_mode(CURRENT_ARG, &mode) == -1) {
			fprintf(stderr, "Invalid mode: %s\n", CURRENT_ARG);
			goto close_error;
        }
		optind++;

        hue_color *colors = NULL;
        int count = 0, min = 0, max = 0;
        switch (mode) {
            case HUE_MODE_FIXED: case HUE_MODE_CANDLE: case HUE_MODE_WINGS: case HUE_MODE_DIRECT:
                min = max = 1;
                break;
            case HUE_MODE_FADING: case HUE_MODE_COVER_MARQUEE:
                min = 2;
                max = 8;
                break;
            case HUE_MODE_SPECTRUM:
                min = max = 0;
                break;
            case HUE_MODE_ALTERNATING:
                min = max = 2;
                break;
            case HUE_MODE_PULSE: case HUE_MODE_BREATHING:
                min = 1;
                max = 8;
                break;
            case HUE_MODE_ALERT: case HUE_MODE_WAVE:
                min = 1;
                max = 2;
                break;
            case HUE_MODE_MARQUEE:
                min = max = 1;
                movement = 0;
                break;
        }

        count = get_colors(argc, argv, &colors, &optind, min, max);
        if (count < 0) {
            fprintf(stderr, "Invalid colors\n");
            free(colors);
            goto close_error;
        }

        enum hue_error ret = hue_set_strip(fd, 1, mode, direction, movement, group, speed, colors, count);
        free(colors);
        switch (ret) {
            case HUE_ERR_NONE:
                break;
            case HUE_ERR_READ:
                fprintf(stderr, "Failed to read\n");
                break;
            case HUE_ERR_LED:
                fprintf(stderr, "No leds\n");
                break;
            case HUE_ERR_WRITE:
                fprintf(stderr, "Failed to write\n");
                break;
            case HUE_ERR_STRIP:
                fprintf(stderr, "Failed to get strip info\n");
                break;
            case HUE_ERR_STATUS:
                fprintf(stderr, "Failed to get status\n");
                break;
            case HUE_ERR_ARG:
                fprintf(stderr, "Invalid arguments\n");
                break;
        }
        goto close;
	} else if (strcmp(action, "init") == 0) {
		hue_init(fd);
		goto close;		
	} else {
		fprintf(stderr, "Unexpected action: %s\n", action);
		goto close_error;
	}
close_error:
	EXIT_CODE = 1;
close:
	close(fd);
	return EXIT_CODE;
}
