#include "main.h"

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

	struct dirent *files[1024];
	struct dirent *folders[1024];
	get_folders_and_files(cwd, folders, files);

	show_folders(w_folders, folders, 2);
	show_files(w_files, files);
	
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

void show_folders(WINDOW *w_folders, struct dirent **folders, int selected)
{
	int i;
	for(i = 0; folders[i] != NULL; i++) {
		if(i == selected) {
			wattron(w_folders, A_REVERSE);
			mvwprintw(w_folders, i+1, 2, "%s", folders[i]->d_name);
			wattroff(w_folders, A_REVERSE);
		} else {
			mvwprintw(w_folders, i+1, 2, "%s", folders[i]->d_name);
		}
	}
}

void show_files(WINDOW *w_files, struct dirent **files)
{
	int i;
	for(i = 0; files[i] != NULL; i++)
		mvwprintw(w_files, i+1, 2, "%2d) %s [%d]", i, files[i]->d_name, files[i]->d_type);
}

