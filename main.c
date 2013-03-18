#include "main.h"

int main(void)
{
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	nodelay(stdscr, TRUE);
	refresh();

	WINDOW *w_path = create_window(1, 1, COLS - 3, 3);
	WINDOW *w_folders = create_window(1, 4, COLS / 3, LINES - 10);
	WINDOW *w_files = create_window(COLS / 3 + 2, 4, 2 * COLS / 3 - 3, LINES - 10);
	
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	mvwprintw(w_path, 1, 2, "%s", cwd);

	struct dirent *folders[1024], *files[1024];
	int num_folders, num_files;
	get_folders_and_files(cwd, folders, files, &num_folders, &num_files);

	int curr_window = FOLDERS;
	int selection = 0;
	int folder_selection = 0, file_selection = 0;
	int input;
	while ((input = getch()) != 'q') {
		clock_t start = clock();

		show_folders(w_folders, folders, folder_selection, curr_window == FOLDERS);
		show_files(w_files, files, file_selection, curr_window == FILES);

		switch (input) {
			case 'j':
				if (curr_window == FOLDERS)
					folder_selection = folder_selection < num_folders - 1 ? 
						folder_selection + 1 : folder_selection;
				else
					file_selection = file_selection < num_files - 1 ? 
						file_selection + 1 : file_selection;
				break;
			case 'k':
				if (curr_window == FOLDERS)
					folder_selection = folder_selection ?
						folder_selection - 1 : folder_selection;
				else
					file_selection = file_selection ?
						file_selection - 1 : file_selection;
				break;
			case '\t':
				curr_window = curr_window ? FOLDERS : FILES;
				break;
			default:
				break;
		}
	
		wrefresh(w_path);
		wrefresh(w_folders);
		wrefresh(w_files);
		refresh();
		lock_fps(start, 60);
	}

	destroy_window(w_path);
	destroy_window(w_folders);
	destroy_window(w_files);

	endwin();
	return 0;
}

void lock_fps(clock_t start, int fps)
{
	struct timespec delay;
	delay.tv_sec = 0;
	delay.tv_nsec = 1000000000 / fps - (clock() - start) * 1000000000 / CLOCKS_PER_SEC;
	nanosleep(&delay, NULL);
}

void show_folders(WINDOW *w_folders, struct dirent **folders, int selection, bool is_active)
{
	int i;
	for (i = 0; folders[i] != NULL; i++) {
		if (is_active && i == selection) {
			wattron(w_folders, A_REVERSE);
			mvwprintw(w_folders, i+1, 2, "%s", folders[i]->d_name);
			wattroff(w_folders, A_REVERSE);
		} else {
			mvwprintw(w_folders, i+1, 2, "%s", folders[i]->d_name);
		}
	}
}

void show_files(WINDOW *w_files, struct dirent **files, int selection, bool is_active)
{
	int i;
	for (i = 0; files[i] != NULL; i++) {
		if (is_active && i == selection) {
			wattron(w_files, A_REVERSE);
			mvwprintw(w_files, i+1, 2, "%2d) %s [%d]", i, files[i]->d_name, files[i]->d_type);
			wattroff(w_files, A_REVERSE);
		} else {
			mvwprintw(w_files, i+1, 2, "%2d) %s [%d]", i, files[i]->d_name, files[i]->d_type);
		}
	}
}

