#include "main.h"

int main(void)
{
	init_ncurses();
	init_screen_params();

	WINDOW *w_path = create_window(1, 1, _w_path_width, 3);
	WINDOW *w_folders = create_window(1, 4, _w_folders_width, _w_folders_height);
	WINDOW *w_files = create_window(_w_folders_width + 2, 4, _w_files_width, _w_files_height);
	
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));

	struct dirent *folders[1024], *files[1024];
	int num_folders, num_files;
	get_folders_and_files(cwd, folders, files, &num_folders, &num_files);

	int curr_window = FOLDERS;
	int selection = 0;
	int folder_selection = 0, file_selection = 0;
	int offset_folders = 0, offset_files = 0;
	int input;
	while ((input = getch()) != 'q') {
		clock_t start = clock();

		mvwprintw(w_path, 1, 2, "%s", cwd);
		
		show_entries(w_folders, FOLDERS, folders, folder_selection, offset_folders, 
				curr_window == FOLDERS, &show_folder);
		show_entries(w_files, FILES, files, file_selection, offset_files, 
				curr_window == FILES, &show_file);

		switch (input) {
			case 'j':
				if (curr_window == FOLDERS)
					move_cursor(w_folders, curr_window, &folder_selection,
							num_folders, &offset_folders, DOWN);
				else
					move_cursor(w_files, curr_window, &file_selection,
							num_files, &offset_files, DOWN);
				break;
			case 'k':
				if (curr_window == FOLDERS)
					move_cursor(w_folders, curr_window, &folder_selection,
							num_folders, &offset_folders, UP);
				else
					move_cursor(w_files, curr_window, &file_selection,
							num_files, &offset_files, UP);
				break;
			case '\t':
				curr_window = curr_window || num_files <= 0 ? FOLDERS : FILES;
				break;
			case 'f':
			case '\n':
				if (curr_window == FOLDERS) {
					char *new_dir = folders[folder_selection]->d_name;
					if (!strcmp(new_dir, "..")) {
						int slash_index = last_index(cwd, '/');
						if(!slash_index)
							snprintf(cwd, sizeof cwd, "/");
						else
							cwd[slash_index] = '\0';
					} else if (strcmp(new_dir, ".")) {
						if(!strcmp(cwd, "/"))
							cwd[0] = '\0';
						snprintf(cwd, sizeof cwd, "%s/%s", cwd, new_dir);
					}
					get_folders_and_files(cwd, folders, files, &num_folders, &num_files);
					folder_selection = file_selection = 0;
					offset_folders = offset_files = 0;
					clear_window(w_path);
					clear_window(w_folders);
					clear_window(w_files);
				}
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



void init_ncurses(void)
{
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	nodelay(stdscr, TRUE);
	refresh();
}

void init_screen_params(void)
{
	_w_path_width = COLS - 1;
	_w_folders_width = COLS / 3;
	_w_folders_height = (LINES - 10)/3;
	_w_files_width = 2 * COLS / 3 - 2;
	_w_files_height = _w_folders_height;
	_max_folders = _w_folders_height - 2;
	_max_files = _w_files_height - 2;
}

void lock_fps(clock_t start, int fps)
{
	struct timespec delay;
	delay.tv_sec = 0;
	delay.tv_nsec = 1000000000 / fps - (clock() - start) * 1000000000 / CLOCKS_PER_SEC;
	nanosleep(&delay, NULL);
}

void show_entries(WINDOW *win, int win_type, struct dirent **entries, int selection, int offset, bool is_active, 
		void (*show)(WINDOW *win, int y, int width, struct dirent *entry, bool is_selected))
{
	int max_entries = win_type ? _max_files : _max_folders;
	int width = win_type ? _w_files_width - 13 : _w_folders_width - 3;
	int i;
	for(i = offset; entries[i] != NULL && i - offset < max_entries; i++) {
		bool is_selected = is_active && i == selection;
		if(is_selected) {
			wattron(win, A_REVERSE);
			show(win, i + 1 - offset, width, entries[i], is_selected);
			wattroff(win, A_REVERSE);
		} else {
			show(win, i + 1 - offset, width, entries[i], is_selected);
		}
	}
}

void show_folder(WINDOW *win, int y, int width, struct dirent *folder, bool is_selected)
{
	char name[1024];
	strcpy(name, folder->d_name);
	if(folder->d_namlen > width)
		name[width] = '\0';
	pad_string(name, width);
	mvwprintw(win, y, 2, "%s", name);
}

void show_file(WINDOW *win, int y, int width, struct dirent *file, bool is_selected)
{
	char name[1024];
	strcpy(name, file->d_name);
	if(file->d_namlen > width)
		name[width] = '\0';
	pad_string(name, width);
	mvwprintw(win, y, 2, "%s [%d]", name, file->d_type);
}

void move_cursor(WINDOW *win, int curr_window, int *selection, int num_entries, int *offset, int dir)
{
	if (dir == DOWN && *selection < num_entries - 1
			|| dir == UP && *selection)
		*selection += dir;
	scroll_window(win, curr_window, selection, offset, dir);
}

void scroll_window(WINDOW *win, int curr_window, int *selection, int *offset, int delta)
{
	int win_height = curr_window ? _w_files_height : _w_folders_height;
	int max_entries = curr_window ? _max_files : _max_folders;
	if(*selection >= max_entries && *selection - *offset >= win_height - 2
		|| *offset > 0 && *selection - *offset < 0)
		*offset += delta;
}

