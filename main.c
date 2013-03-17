#include <ncurses.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

WINDOW *create_window(int x, int y, int width, int height);
void destroy_window(WINDOW *win);

int main(void)
{
	initscr();
	cbreak();
	noecho();
	refresh();

	WINDOW *w_path = create_window(1, 1, COLS - 3, 3);
	WINDOW *w_folders = create_window(1, 4, COLS / 3, LINES - 10);
	WINDOW *w_files = create_window(COLS / 3 + 2, 4, 2 * COLS / 3 - 3, LINES - 10);
	
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	mvwprintw(w_path, 1, 2, "%s", cwd);
	
	DIR *dir = opendir(cwd);
	if(dir != NULL) {
		int i;
		struct dirent *entry;
		for(i = 1; entry = readdir(dir); i++)
			mvwprintw(w_folders, i, 1, "%s", entry->d_name);
		closedir(dir);
	}

	wrefresh(w_path);
	wrefresh(w_folders);
	wrefresh(w_files);
	refresh();
	getch();

	destroy_window(w_path);
	destroy_window(w_folders);
	destroy_window(w_files);

	endwin();
	return 0;
}

WINDOW *create_window(int x, int y, int width, int height)
{
	WINDOW *win = newwin(height, width, y, x);
	box(win, 0, 0);
	wrefresh(win);
	return win;
}

void destroy_window(WINDOW *win)
{
	wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(win);
	delwin(win);
}
