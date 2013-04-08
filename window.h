#include <ncurses.h>
#include "util.h"

WINDOW *create_window(int x, int y, int width, int height);
void clear_window(WINDOW *win);
void destroy_window(WINDOW *win);
void draw_border(WINDOW *win);
void draw_title(WINDOW *win, char *title);

