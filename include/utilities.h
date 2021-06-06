#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <SDL.h>

#define DELAY_DEFAULT 1000
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define BACKGROUND_COLOR_R_DEFAULT 255
#define BACKGROUND_COLOR_G_DEFAULT 240
#define BACKGROUND_COLOR_B_DEFAULT 240

extern char *proj_dir;
extern char mode;
extern int step;

struct background_meta_data {
	int width;
	int height;
	int color_r;
	int color_g;
	int color_b;
};
extern struct background_meta_data bg_meta;

char *strremove(char *str, const char *sub, int trunc);
char *get_proj_dir(char *command);
static void print_usage(void);
static void print_patterns(char *pattern_choices[]);
char *parse_pattern_choice(void);
void parse_input(int argc, char *argv[]);
void display_text(SDL_Renderer *renderer, char *text, SDL_Color color, int font_size, int x, int y, int w, int h);
void display_body_statistics(SDL_Renderer *renderer, int gen, int pop);

#define DISPLAY_STAT(renderer, text, color, height) display_text(renderer, text, color, 18, 25, height, 0, 0);

#endif /* _UTILITIES_H_ */
