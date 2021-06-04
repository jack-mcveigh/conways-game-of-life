#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "utilities.h"
#include "cell.h"

char *proj_dir;
char mode = 'r';
int step = 0;

struct cell_meta_data cell_meta = {
	.rows = CELL_ROWS_DEFAULT,
	.cols = CELL_COLS_DEFAULT,
	.width = CELL_WIDTH_DEFAULT,
	.height = CELL_HEIGHT_DEFAULT,
	.alive_prob = CELL_SPAWN_PROBABILITY_DEFAULT,
	.color_r = CELL_COLOR_R_DEFAULT,
	.color_g = CELL_COLOR_G_DEFAULT,
	.color_b = CELL_COLOR_B_DEFAULT
};

struct background_meta_data bg_meta = {
	.width = WINDOW_WIDTH,
	.height = WINDOW_HEIGHT,
	.color_r = BACKGROUND_COLOR_R_DEFAULT,
	.color_g = BACKGROUND_COLOR_G_DEFAULT,
	.color_b = BACKGROUND_COLOR_G_DEFAULT,
};

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
	body = body_init(cell_meta.rows, cell_meta.cols);
	body_old = body_init(cell_meta.rows, cell_meta.cols);
	inital_generation(body, body_old, &population);

	/* Initialize TTF */
	TTF_Init();

	/* Initialize window */
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("Conway's Game of Life - Jack McVeigh", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, bg_meta.width, bg_meta.height, SDL_WINDOW_OPENGL);
	if (!window) {
		perror("main: Failed to create window");
		exit(EXIT_FAILURE);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!window) {
		perror("main: Failed to create renderer");
		exit(EXIT_FAILURE);
	}
	SDL_SetRenderDrawColor(renderer, bg_meta.color_r, bg_meta.color_g, bg_meta.color_b, SDL_ALPHA_OPAQUE); /* salmon-ish */

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

			/* Ping-pong buffer */
			temp = body->cells;
			body->cells = body_old->cells;
			body_old->cells = temp;

			/* Compute next generation */
			compute_generation(body, body_old, &population);
			generation++;

			if (step)
				pause = 1;
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
						case SDLK_q:
							done = 1;
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
