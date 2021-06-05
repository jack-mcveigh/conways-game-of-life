#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "utilities.h"
#include "cell.h"

char *strremove(char *str, const char *sub, int trunc)
{
	char *p, *q, *r;
	if ((q = r = strstr(str, sub)) != NULL) {
		size_t len = strlen(sub);
		while ((r = strstr(p = r + len, sub)) != NULL) {
			while (p < r)
			*q++ = *p++;
		}
		if (!trunc)
			while((*q++ = *p++) != '\0');
		else
			*q++ = '\0';
	}
	return str;
}

char *get_proj_dir(char *command)
{
	char *resolved_path = malloc(PATH_MAX);
	realpath(command, resolved_path);
	return strremove(resolved_path, "/bin", 1);
}

static void print_usage(void)
{
        printf("./game_of_life [ -h | [-n] [-d] ]\n");
        printf("optional arguments:\n");
        printf("\t-h\t\t: Print the usage statement.\n");
	printf("\t-s\t\t: Stepper mode, pause after each frame.\n");
        printf("\t-n\t\t: Number of cells. (nxn)\n");
        printf("\t-d\t\t: Dimensions of the body matrix. (dxd)\n");
	printf("\t-p\t\t: Probability of cell being alive. (p%%)\n");
	printf("\t-c\t\t: Set cell color. (0xRRGGBB)\n");
	printf("\t-b\t\t: Set background color. (0xRRGGBB)\n");
	printf("\t-m\t\t: Select mode. (r: random, p: pattern, d: drawing)\n");
}

static void print_patterns(char *pattern_choices[])
{
	int i;

	printf("Available Patterns:\n");

	for (i=0; pattern_choices[i] != NULL; i++)
		printf("\t%d: %s\n", i+1, pattern_choices[i]);
}

char *parse_pattern_choice(void)
{
	int choice;
	char *pattern_path;
	char *pattern_rel_path;
	char *pattern_choices[] = {
		"/data/patterns/death/stick.csv",
		"/data/patterns/oscillation/crosshair.csv",
		"/data/patterns/stable/convergent/block.csv",
		"/data/patterns/stable/convergent/circle.csv",
		"/data/patterns/stable/convergent/oval.csv",
		"/data/patterns/stable/divergent/glider.csv",
		NULL
	};

	print_patterns(pattern_choices);
	printf("Please select a pattern: ");
	scanf("%d", &choice);
	choice--;

	pattern_rel_path = malloc(strlen(pattern_choices[choice]));
	strcpy(pattern_rel_path, pattern_choices[choice]);

	pattern_path = malloc(strlen(proj_dir) + strlen(pattern_rel_path) + 1);
	strcpy(pattern_path, proj_dir);
	strcat(pattern_path, pattern_rel_path);

	return pattern_path;
}

int parse_input(int argc, char *argv[])
{
	int option, n, d;
	n = 0;
	d = 0;

	proj_dir = get_proj_dir(argv[0]);

	while ((option = getopt(argc, argv, ":hsn:d:p:c:b:m:")) != -1) {
		switch (option) {
			case 'h': /* Print usage */
				goto usage_and_exit;
			case 's': /* Step mode */
				step = 1;
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
				if (mode != 'r' && mode != 'p' && mode != 'd') { /* Invalid mode */
					fprintf(stderr, "Not a valid option (valid options are r: random, p: pattern, d: drawing).\n");
					goto usage_and_exit;
				}
				break;
			case ':': /* Needs value */
				fprintf(stderr, "Option needs value.\n");
				goto usage_and_exit;
			case '?': /* Unknown */
				fprintf(stderr, "Invalid option.\n");
				goto usage_and_exit;
		}
	}

	if ((cell_meta.width * cell_meta.rows != WINDOW_WIDTH)
	    || (cell_meta.height * cell_meta.cols != WINDOW_HEIGHT)) /* Invalid cell dims. */
		goto usage_and_exit;
	else if ((cell_meta.alive_prob > 100) || (cell_meta.alive_prob < 0)) /* Prob. must be percentage 0-100 */
		goto usage_and_exit;

	for(; optind < argc; optind++) { /* Extra args */
		fprintf(stderr, "Invalid option %s.\n", argv[optind]);
		goto usage_and_exit;
	}
        return 0;

usage_and_exit:
	print_usage();
	exit(EXIT_FAILURE);
}

void display_body_statistics(SDL_Renderer *renderer, int gen, int pop)
{
	char text[124];
	SDL_Color color = {101, 101, 101}; /* Light Gray */

	sprintf(text, "Current Generation: %d", gen);
	display_text(renderer, text, color, 25, 25, 0, 0);

	sprintf(text, "Population: %d", pop);
	display_text(renderer, text, color, 25, 50, 0, 0);
}

void display_text(SDL_Renderer *renderer, char *text, SDL_Color color, int x, int y, int w, int h)
{
	TTF_Font* font;
	SDL_Surface* surface_message;
	SDL_Texture* texture_message;
	SDL_Rect message_rect;
	char *font_path;
	char font_rel_path[] = "/data/assets/Arial.ttf";

	font_path = malloc(strlen(proj_dir) + strlen(font_rel_path) + 1);
	strcpy(font_path, proj_dir);
	strcat(font_path, font_rel_path);

	font = TTF_OpenFont(font_path, 18);
	if (!font) {
		printf("%s\n", TTF_GetError());
		perror("display_body_statistics: TTF_OpenFont failed");
		exit(EXIT_FAILURE);
	}
	free(font_path);

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
