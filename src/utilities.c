#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "utilities.h"
#include "cell.h"

/*
 * Function:	strremove
 * ----------------------
 * Find substring in a string and remove it, tuncating the
 * 	rest of the string if desired.
 *
 * str: the main string.
 * sub: the desired substring to be removed.
 * trunc: 1 if truncate the rest of the string, 0 if just remove substring.
 *
 * returns: pointer to the original string with the changes made.
 */
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

/*
 * Function:	get_proj_dir
 * -------------------------
 * Get the project dir given the command call (argv[0]).
 *
 * command: the command from argv[0].
 *
 * returns: pointer to newly allocated string containing the project path.
 */
char *get_proj_dir(char *command)
{
	char *resolved_path = malloc(PATH_MAX);
	if (!resolved_path) {
		perror("get_proj_dir: Failed to malloc resolved_path");
		exit(EXIT_FAILURE);
	}
	realpath(command, resolved_path);
	return strremove(resolved_path, "/bin", 1);
}

/*
 * Function:	print_usage
 * ------------------------
 * Print the program usage.
 */
static void print_usage(void)
{
        printf("usage: ./game_of_life [-h | [-sgn:d:p:c:b:m:]]\n");
	printf("\t: - needs value\n");
        printf("optional arguments:\n");
        printf("\t-h\t\t: Print the usage statement.\n");
	printf("\t-s\t\t: Stepper mode, pause after each frame.\n");
	printf("\t-g\t\t: Grid on.\n");
        printf("\t-n\t\t: Number of cells. (nxn)\n");
        printf("\t-d\t\t: Dimensions of the body matrix. (dxd)\n");
	printf("\t-p\t\t: Probability of cell being alive. (p%%)\n");
	printf("\t-c\t\t: Set cell color. (0xRRGGBB)\n");
	printf("\t-b\t\t: Set background color. (0xRRGGBB)\n");
	printf("\t-m\t\t: Select mode. (r: random, p: pattern, d: drawing)\n");
}

/*
 * Function:	print_patterns
 * ---------------------------
 * Print the available pattens.
 *
 * pattern_choices: arracy containing the available patterns.
 */
static void print_patterns(char *pattern_choices[])
{
	int i;

	printf("Available Patterns:\n");

	for (i=0; pattern_choices[i] != NULL; i++)
		printf("\t%d: %s\n", i+1, pattern_choices[i]);
}

/*
 * Function:	parse_pattern_choice
 * ---------------------------------
 * Get the pattern the user wants to use.
 *
 * returns: path to the pattern that the user selected.
 */
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
	choice--; /* map 1:n to 0:n-1 for array indexing */

	pattern_rel_path = malloc(strlen(pattern_choices[choice]));
	if (!pattern_rel_path) {
		perror("parse_pattern_choice: Failed to malloc pattern_rel_path");
		exit(EXIT_FAILURE);
	}
	strcpy(pattern_rel_path, pattern_choices[choice]);

	pattern_path = malloc(strlen(proj_dir) + strlen(pattern_rel_path) + 1);
	if (!pattern_path) {
		perror("parse_pattern_choice: Failed to malloc pattern_path");
		free(pattern_rel_path);
		exit(EXIT_FAILURE);
	}
	strcpy(pattern_path, proj_dir);
	strcat(pattern_path, pattern_rel_path);

	return pattern_path;
}

/*
 * Function:	parse_input
 * ------------------------
 * Parse the command line input
 *
 * argc: argument count.
 * argv: array of argument strings.
 */
void parse_input(int argc, char *argv[])
{
	int option;

	proj_dir = get_proj_dir(argv[0]);

	while ((option = getopt(argc, argv, ":hsgn:d:p:c:b:m:")) != -1) {
		switch (option) {
			case 'h': /* Print usage */
				goto usage_and_exit;
			case 's': /* Step mode */
				step = 1;
				break;
			case 'g': /* Grid on */
				cell_meta.grid_on = 1;
				break;
			case 'n': /* Number of cells */
				cell_meta.rows = cell_meta.cols = atoi(optarg);
				break;
			case 'd': /* Cell dimensions */
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
					fprintf(stderr, "game_of_life: not a valid option (valid options are r: random, p: pattern, d: drawing).\n");
					goto usage_and_exit;
				}
				break;
			case ':': /* Needs value */
				fprintf(stderr, "game_of_life: option needs value.\n");
				goto usage_and_exit;
			case '?': /* Unknown */
				fprintf(stderr, "game_of_life: invalid option.\n");
				goto usage_and_exit;
		}
	}

	bg_meta.width = cell_meta.width * cell_meta.cols;
	bg_meta.height = cell_meta.height * cell_meta.rows;

	if ((bg_meta.width > MAX_WINDOW_WIDTH) || (bg_meta.height > MAX_WINDOW_HEIGHT) ||
	    (bg_meta.width < MIN_WINDOW_WIDTH) || (bg_meta.height < MIN_WINDOW_HEIGHT) ) { /* Invalid cell dims. */
		fprintf(stderr, "game_of_life: invalid window size (too small/large)\n");
		goto usage_and_exit;
	}
	else if ((cell_meta.alive_prob > 100) || (cell_meta.alive_prob < 0)) { /* Prob. must be percentage 0-100 */
		fprintf(stderr, "game_of_life: probability value must be a 0-100\n");
		goto usage_and_exit;
	}

	for(; optind < argc; optind++) { /* Extra args */
		fprintf(stderr, "game_of_life: invalid option %s.\n", argv[optind]);
		goto usage_and_exit;
	}

	return;

usage_and_exit:
	print_usage();
	exit(EXIT_FAILURE);
}

/*
 * Function:	display_text
 * -------------------------
 * Display text on the window.
 *
 * renderer: SDL_Renderer used for rendering the text.
 * color: the color for rendering the text.
 * x: the column to start rendering the text.
 * y: the row to start rendering the text.
 * w: the width of the text box.
 * h: the height of the text box.
 */
void display_text(SDL_Renderer *renderer, char *text, SDL_Color color, int font_size, int x, int y, int w, int h)
{
	TTF_Font* font;
	SDL_Surface* surface_message;
	SDL_Texture* texture_message;
	SDL_Rect message_rect;
	char *font_path;
	char font_rel_path[] = "/data/assets/Arial.ttf";

	font_path = malloc(strlen(proj_dir) + strlen(font_rel_path) + 1);
	if (!font_path) {
		perror("display_text: Failed to malloc font_path");
		exit(EXIT_FAILURE);
	}
	strcpy(font_path, proj_dir);
	strcat(font_path, font_rel_path);

	font = TTF_OpenFont(font_path, font_size);
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

	TTF_CloseFont(font);
	SDL_FreeSurface(surface_message);
	SDL_DestroyTexture(texture_message);
}

/*
 * Function:	display_body_statistics
 * ------------------------------------
 * Display the current generation and population.
 *
 * renderer: SDL_Renderer used for rendering the statistics.
 * gen: the current generation count.
 * pop: the current population count.
 */
void display_body_statistics(SDL_Renderer *renderer, int gen, int pop)
{
	char text[124];
	SDL_Color color;

	if (cell_meta.grid_on)
		color.r = color.g = color.b = 0; /* Black */
	else
		color.r = color.g = color.b = 101; /* Light Gray */

	sprintf(text, "Current Generation: %d", gen);
	DISPLAY_STAT(renderer, text, color, 25);

	sprintf(text, "Population: %d", pop);
	DISPLAY_STAT(renderer, text, color, 50);
}
