#include <ncurses.h>

WINDOW *create_window(int x, int y, int width, int height);
void destroy_window(WINDOW *win);
