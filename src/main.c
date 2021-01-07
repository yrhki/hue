#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>

#include "serial.h"
#include "baudrate.h"
#include "hue.h"

int parse_color(char* hexstring, Color* color) {
	size_t length = strlen(hexstring);
	if (length == 3 || length == 6) {
		int delim = length / 3;
		char red[3] = {0};
		char green[3] = {0};
		char blue[3] = {0};

		memcpy(red, hexstring, delim * sizeof(char));
		strncpy(green, hexstring + (delim * 1), delim);
		strncpy(blue, hexstring + (delim * 2), delim);
	
		color->Red = strtol(red, NULL, 16); 
		color->Green = strtol(green, NULL, 16); 
		color->Blue = strtol(blue, NULL, 16); 
		return 0;
	} else {
		return -1;
	}
}


int main(int argc, char* argv[]) {

	char* name = argv[0];
	char* portname = "/dev/ttyACM0";
	int mode = 0;
	int EXIT_CODE = 0;

	while (1) {
		static struct option long_options[] =
		{
			//{"verbose",	no_argument,	&verbose_flag, 'V'}
			{"version", no_argument,		0,	'v'},
			{"help",	no_argument,		0,	'h'},
			{"serial",	required_argument,	0,	's'},
			
			{0,0,0,0}
		};
		int c, option_index;
		//int this_option_optind = optind ? optind : 1;

		c = getopt_long(argc, argv, "vhs:", long_options, &option_index);
		if (c == -1) break;
		switch (c) {
			case 's':
				printf("SERIAL: %s\n", optarg);
				portname = optarg;
				break;
			case 'v':
				break;
			default:
				fprintf(stderr, "Usage: %s [-s serial] mode colors\n", name);
				exit(EXIT_FAILURE);
		}
	}	

	char* color_hex = {0};

	if (optind >= argc) {
		fprintf(stderr, "Expected argument after options\n");
		exit(EXIT_FAILURE);
	}

	int fd = open_serial(portname);
	if (fd < 0) return 1;
	set_interface_attribs(fd);
	set_custom_baudrate(fd);
	char* action = argv[optind];

	if (strcmp(action, "mode") == 0) {
		optind++;
		if (optind >= argc) {
			fprintf(stderr, "No mode passed\n");
			goto close_error;
		}
		char* amode = argv[optind];

		if (strcmp(amode, "fixed") == 0)
			mode = FIXED;
		else if (strcmp(amode, "fading") == 0)
			mode = FADING;
		else if (strcmp(amode, "spectrum") == 0)
			mode = SPECTRUM;
		else if (strcmp(amode, "marquee") == 0)
			mode = MARQUEE;
		else if (strcmp(amode, "cmarquee") == 0)
			mode = COVER_MARQUEE;
		else if (strcmp(amode, "alternating") == 0)
			mode = ALTERNATING;
		else if (strcmp(amode, "pulse") == 0)
			mode = PULSE;
		else if (strcmp(amode, "breathing") == 0)
			mode = BREATHING;
		else if (strcmp(amode, "alert") == 0)
			mode = ALERT;
		else if (strcmp(amode, "candle") == 0)
			mode = CANDLE;
		else if (strcmp(amode, "wings") == 0)
			mode = WINGS;
		else if (strcmp(amode, "wave") == 0)
			mode = WAVE;
		else {
			fprintf(stderr, "Invalid mode: %s\n", amode);
			//exit(EXIT_FAILURE);
			goto close_error;
		}
		optind++;
		if (optind >= argc) {
			fprintf(stderr, "No color passed\n");
			goto close_error;
		}

		//init(fd);
		int color_len = argc - optind;
		Color* input_colors = NULL;
		input_colors = realloc(input_colors, color_len * sizeof *input_colors);
		if (input_colors == NULL)
			goto close_error;

		for ( ; optind  < argc; ++optind) {
			color_hex = argv[argc - color_len + (argc - optind -1)];
			Color input_color = {0};
			if (parse_color(color_hex, &input_color) != 0) {
				fprintf(stderr, "Invalid color: %s\n", color_hex);
				free(input_colors);
				goto close_error;
			}
			input_colors[argc - optind - 1] = input_color;
		}

		set_strip(fd, 1, mode, 0, 0, 0, 0, input_colors, color_len);
		free(input_colors);
		goto close;		


	} else if (strcmp(action, "init") == 0) {
		uint8_t resp = init(fd);
		printf("INIT: %d\n", resp);
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
