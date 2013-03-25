#include "main.h"

int main(void)
{
	init_ncurses();
	init_screen_params();

	WINDOW *w_path = create_window(1, 1, _w_path_width, 3);
	WINDOW *w_folders = create_window(1, 4, _w_folders_width, _w_folders_height);
	WINDOW *w_files = create_window(_w_folders_width + 2, 4, _w_files_width, _w_files_height);
	WINDOW *windows[] = {w_path, w_folders, w_files, 0};
	
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));

	struct fs_entry *folders[1024], *files[1024];
	malloc_entries(folders, 1024);
	malloc_entries(files, 1024);
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
		draw_scrollbar(w_folders, num_folders, _w_folders_width - 2, _w_folders_height - 2, offset_folders);
		draw_scrollbar(w_files, num_files, _w_files_width - 2, _w_files_height - 2, offset_files);
		draw_title(w_folders, "folders");
		draw_title(w_files, "files");

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
					char *new_dir = folders[folder_selection]->ent->d_name;
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
					//next_dir(cwd, new_dir);
					get_folders_and_files(cwd, folders, files, &num_folders, &num_files);
					folder_selection = file_selection = 0;
					offset_folders = offset_files = 0;
					clear_windows(windows);
				} else {
					char cmd[1024], command[1024];
					if (files[file_selection]->can_exec) {
						snprintf(cmd, sizeof cmd, "%s/%s", cwd, 
							files[file_selection]->ent->d_name);
						escape_path(cmd, command, false);
					} else {
						snprintf(cmd, sizeof cmd, "vim %s/%s", cwd, 
							files[file_selection]->ent->d_name);
						escape_path(cmd, command, true);
					}
					run_command(command);
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

	clear();
	wclear(w_path);
	wclear(w_folders);
	wclear(w_files);
	destroy_window(w_path);
	destroy_window(w_folders);
	destroy_window(w_files);

	endwin();
	return 0;
}

void next_dir(char *path, char *new_dir)
{
	if (!strcmp(new_dir, "..")) {
		int slash_index = last_index(path, '/');
		if(!slash_index)
			snprintf(path, sizeof path, "/");
		else
			path[slash_index] = '\0';
	} else if (strcmp(new_dir, ".")) {
		if(!strcmp(path, "/"))
			path[0] = '\0';
		snprintf(path, sizeof path, "%s/%s", path, new_dir);
	}
}

void init_ncurses(void)
{
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	nodelay(stdscr, TRUE);
	refresh();
	init_colors();
}

void init_colors(void)
{
	start_color();
	init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
	init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
	init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void init_screen_params(void)
{
	_w_path_width = COLS - 2;
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

void show_entries(WINDOW *win, int win_type, struct fs_entry **entries, int selection, int offset, bool is_active, 
		void (*show)(WINDOW *win, int y, int width, struct fs_entry *entry, bool is_selected))
{
	int max_entries = win_type ? _max_files : _max_folders;
	int width = win_type ? _w_files_width - 13 : _w_folders_width - 3;
	int i;
	for(i = offset; entries[i]->ent != NULL && i - offset < max_entries; i++) {
		bool is_selected = is_active && i == selection;
		if (is_selected) {
			wattron(win, A_REVERSE);
			show(win, i + 1 - offset, width, entries[i], is_selected);
			wattroff(win, A_REVERSE);
		} else {
			show(win, i + 1 - offset, width, entries[i], is_selected);
		}
	}
}

void show_folder(WINDOW *win, int y, int width, struct fs_entry *folder, bool is_selected)
{
	int color = folder->can_read ? COLOR_WHITE : COLOR_BLUE;
	wattron(win, COLOR_PAIR(color));
	mvwprintw(win, y, 2, "%-*.*s", width, width, folder->ent->d_name);
	wattroff(win, COLOR_PAIR(color));
}

void show_file(WINDOW *win, int y, int width, struct fs_entry *file, bool is_selected)
{
	int color;
	if (!file->can_read)
		color = COLOR_BLUE;
	else if (file->ent->d_type != DT_REG)
		color = COLOR_CYAN;
	else if (file->can_exec)
		color = COLOR_MAGENTA;
	else
		color = COLOR_WHITE;
	char permissions[4];
	permissions[0] = file->can_read ? 'r' : ' ';
	permissions[1] = file->can_write ? 'w' : ' ';
	permissions[2] = file->can_exec ? 'x' : ' ';
	permissions[3] = '\0';
	wattron(win, COLOR_PAIR(color));
	mvwprintw(win, y, 2, "%-*.*s [%s %d]", width, width, file->ent->d_name,
			permissions, file->ent->d_type);
	wattroff(win, COLOR_PAIR(color));
}

void draw_scrollbar(WINDOW *win, int num_entries, int x, int win_height, int offset)
{
	if (num_entries > win_height) {
		float sb_ratio = (float)win_height / (float)num_entries;
		int sb_height = sb_ratio * win_height + 2;
		int sb_offset = sb_ratio * offset;
		wattron(win, COLOR_PAIR(COLOR_YELLOW));
		int y;
		for (y = 1; y <= win_height; y++) {
			char c = y > sb_offset && y < sb_offset + sb_height ? '#' : ' ';
			mvwprintw(win, y, x, "%c", c);
		}
		wattroff(win, COLOR_PAIR(COLOR_YELLOW));
	}
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

void run_command(char *command)
{
	def_prog_mode();
	endwin();
	system(command);
	reset_prog_mode();
}
