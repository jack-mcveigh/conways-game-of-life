#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "utilities.h"
#include "cell.h"

static void print_usage(void)
{
        printf("./game_of_life [ -h | [-n] [-d] ]\n");
        printf("optional arguments:\n");
        printf("\t-h\t\t: Print the usage statement.\n");
        printf("\t-n\t\t: Number of cells. (nxn)\n");
        printf("\t-d\t\t: Dimensions of the body matrix. (dxd)\n");
	printf("\t-p\t\t: Probability of cell being alive. (p%%)\n");
	printf("\t-c\t\t: Set cell color. (0xRRGGBB)\n");
	printf("\t-b\t\t: Set background color. (0xRRGGBB)\n");
}

static void print_patterns(void)
{
	/* TODO: Walk ../data/patterns for pattern options */

	return;
}

char *parse_pattern_choice(void)
{
	print_patterns();

	/* TODO: return the full path to the file that the user requested. */

	return (char*)NULL;
}

int parse_input(int argc, char *argv[])
{
	int option, n, d;
	n = 0;
	d = 0;

	while ((option = getopt(argc, argv, ":hn:d:p:c:b:")) != -1) { //get option from the getopt() method
		switch (option) {
			case 'h': /* Print usage */
				print_usage();
				exit(EXIT_SUCCESS);
				break;
			case 'n': /* Number of cells */
				n = 1;
				cell_meta.rows = cell_meta.cols = atoi(optarg);
				break;
			case 'd': /* Cell dimensions */
				d = 1;
				cell_meta.width = cell_meta.height = atoi(optarg);
				break;
			case 'p': /* Alive Probability */
				cell_meta.alive_prob = atoi(optarg);
				break;
			case 'c': /* Cell color */
				cell_meta.color_r = (strtol(optarg, NULL, 16) >> 16) & 0xFF;
				cell_meta.color_g = (strtol(optarg, NULL, 16) >> 8) & 0xFF;
				cell_meta.color_b = strtol(optarg, NULL, 16) & 0xFF;
				break;
			case 'b': /* Background color */
				bg_meta.color_r = (strtol(optarg, NULL, 16) >> 16) & 0xFF;
				bg_meta.color_g = (strtol(optarg, NULL, 16) >> 8) & 0xFF;
				bg_meta.color_b = strtol(optarg, NULL, 16) & 0xFF;
				break;
			case 'm': /* Background color */
				mode = optarg[0];
				assert(mode == 'r' || mode == 'p' || mode == 'd'); /* Invalid mode */
				break;
			case ':': /* Needs value */
				fprintf(stderr, "Option needs value.\n");
				print_usage();
				exit(EXIT_FAILURE);
				break;
			case '?': /* Unknown */
				fprintf(stderr, "Invalid option.\n");
				print_usage();
				exit(EXIT_FAILURE);
				break;
		}
	}


	assert((cell_meta.width * cell_meta.rows == WINDOW_WIDTH)
	       && (cell_meta.height * cell_meta.cols == WINDOW_HEIGHT)); /* Invalid cell dims. */
	assert((cell_meta.alive_prob < 100) && (cell_meta.alive_prob > 0)); /* Prob. must be percentage */

	for(; optind < argc; optind++) { /* Extra args */
		fprintf(stderr, "Invalid option %s.\n", argv[optind]);
		print_usage();
		exit(EXIT_FAILURE);
	}
        return 0;
}

void display_text(SDL_Renderer *renderer, char *text, SDL_Color color, int x, int y, int w, int h)
{
	TTF_Font* font;
	SDL_Surface* surface_message;
	SDL_Texture* texture_message;
	SDL_Rect message_rect;

	font = TTF_OpenFont("../data/assets/Arial.ttf", 18);
	if (!font) {
		printf("%s\n", TTF_GetError());
		perror("display_body_statistics: TTF_OpenFont failed");
		exit(EXIT_FAILURE);
	}

	surface_message = TTF_RenderText_Solid(font, text, color);
	if (!surface_message) {
		perror("display_body_statistics: TTF_RenderText_Solid failed");
		exit(EXIT_FAILURE);
	}

	texture_message = SDL_CreateTextureFromSurface(renderer, surface_message);
	if (!texture_message) {
		perror("display_body_statistics: SDL_CreateTextureFromSurface failed");
		exit(EXIT_FAILURE);
	}

	message_rect.x = x;
	message_rect.y = y;
	message_rect.w = w;
	message_rect.h = h;
	if (SDL_QueryTexture(texture_message, NULL, NULL, &message_rect.w, &message_rect.h) == -1) {
		perror("display_body_statistics: SDL_QueryTexture failed");
		exit(EXIT_FAILURE);
	}

	if (SDL_RenderCopy(renderer, texture_message, NULL, &message_rect) == -1) {
		perror("display_body_statistics: SDL_RenderCopy failed to display the message");
		exit(EXIT_FAILURE);
	}

	SDL_FreeSurface(surface_message);
	SDL_DestroyTexture(texture_message);
}
