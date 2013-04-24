#include "window.h"

WINDOW *create_window(int x, int y, int width, int height)
{
	WINDOW *win = newwin(height, width, y, x);
	draw_border(win);
	wrefresh(win);
	return win;
}

void clear_windows(WINDOW **windows)
{
	int i;
	for(i = 0; windows[i]; i++)
		clear_window(windows[i]);
}

void clear_window(WINDOW *win)
{
	werase(win);
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

void draw_title(WINDOW *win, char *title)
{
	wattron(win, COLOR_PAIR(COLOR_CYAN));
	wprint_str(win, 2, 0, title);
	wattroff(win, COLOR_PAIR(COLOR_CYAN));
}

