#include "window.h"

WINDOW *create_window(int x, int y, int width, int height)
{
	WINDOW *win = newwin(height, width, y, x);
	box(win, 0, 0);
	wrefresh(win);
	return win;
}

void clear_window(WINDOW *win)
{
	wclear(win);
	box(win, 0, 0);
}

void destroy_window(WINDOW *win)
{
	wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(win);
	delwin(win);
}
