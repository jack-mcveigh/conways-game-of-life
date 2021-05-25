#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "cell.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define DELAY_DEFAULT 1000

int cell_rows = CELL_ROWS_DEFAULT;
int cell_cols = CELL_COLS_DEFAULT;
int cell_width = CELL_WIDTH_DEFAULT;
int cell_height = CELL_HEIGHT_DEFAULT;

void print_usage(void);
int parse_input(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	uint32_t delay_interval;
	int done, pause, generation, population;
	cell_t **temp;
	body_t *body, *body_old;
	SDL_Window* window;
	SDL_Renderer *renderer;
	SDL_Event event;

	srand(time(0));

	parse_input(argc, argv);

	/* Initialize bodies */
	temp = malloc(sizeof(*temp));
	body = body_init(cell_rows, cell_cols);
	body_old = body_init(cell_rows, cell_cols);
	inital_generation(body, body_old, &population);

	/* Initialize TTF */
	TTF_Init();

	/* Initialize window */
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("Conway's Game of Life - Jack McVeigh", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	if (!window) {
		perror("main: Failed to create window");
		exit(EXIT_FAILURE);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!window) {
		perror("main: Failed to create renderer");
		exit(EXIT_FAILURE);
	}
	SDL_SetRenderDrawColor(renderer, 255, 240, 240, SDL_ALPHA_OPAQUE); /* salmon-ish */

	/* Main loop */
	done = pause = generation = 0;
	delay_interval = DELAY_DEFAULT;
	while (!done) {
		if (!pause) {
			/* Render */
			SDL_RenderClear(renderer);
			draw_generation(renderer, body);
			display_body_statistics(renderer, generation, population);
			SDL_RenderPresent(renderer);

			/* Compute next generation */
			compute_generation(body, body_old, &population);
			generation++;

			/* Ping-pong buffer */
			temp = body->cells;
			body->cells = body_old->cells;
			body_old->cells = temp;
		}

		SDL_Delay(delay_interval);

		/* Poll for events */
		while (SDL_PollEvent(&event))
			switch (event.type) {
				case SDL_QUIT:
					done = 1;
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_SPACE:
							pause = !pause;
							break;
						case SDLK_UP:
							if (delay_interval >= 100)
								delay_interval -= 100;
							break;
						case SDLK_DOWN:
							if (delay_interval < (DELAY_DEFAULT * 4))
								delay_interval += 100;
							break;
					}
					break;
			}
	}

	/* Destory program */
	body_destory(body);
	body_destory(body_old);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

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
