#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "utilities.h"
#include "cell.h"

void print_usage(void)
{
        printf("./game_of_life [ -h | [-n] [-d] ]\n");
        printf("optional arguments:\n");
        printf("\t-h\t\t: Print the usage statement.\n");
        printf("\t-n\t\t: Number of cells. (nxn)\n");
        printf("\t-d\t\t: Dimensions of the body matrix. (dxd)\n");
}

int parse_input(int argc, char *argv[])
{
	int option, n, d;
	n = 0;
	d = 0;

	while((option = getopt(argc, argv, ":hn:d:")) != -1) { //get option from the getopt() method
		switch(option) {
			case 'h': /* Print usage */
				print_usage();
				exit(EXIT_SUCCESS);
				break;
			case 'n': /* Number of cells */
				n = 1;
				cell_rows = cell_cols = atoi(optarg);
				break;
			case 'd': /* Cell dimensions */
				d = 1;
				cell_width = cell_height = atoi(optarg);
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

	if (n && d)
		assert((cell_width * cell_rows == WINDOW_WIDTH) && (cell_height * cell_cols == WINDOW_HEIGHT));

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

	font = TTF_OpenFont("../assets/Arial.ttf", 18);
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
