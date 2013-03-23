#include "window.h"

WINDOW *create_window(int x, int y, int width, int height)
{
	WINDOW *win = newwin(height, width, y, x);
	draw_border(win);
	wrefresh(win);
	return win;
}

void clear_window(WINDOW *win)
{
	wclear(win);
	draw_border(win);
}

void destroy_window(WINDOW *win)
{
	wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(win);
	delwin(win);
}

void draw_border(WINDOW *win)
{
	wattron(win, COLOR_PAIR(COLOR_CYAN));
	box(win, 0, 0);
	wattroff(win, COLOR_PAIR(COLOR_CYAN));
}

