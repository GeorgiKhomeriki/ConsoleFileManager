#include "main.h"

int main(void)
{
	setlogmask(LOG_UPTO(LOG_NOTICE));
	openlog("cfm", LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog(LOG_NOTICE, "Started Console File Manager");

	init_ncurses();
	init_screen_params();

	WINDOW *w_path = create_window(1, 1, _w_path_width, 3);
	WINDOW *w_folders = create_window(1, 4, _w_folders_width, _w_folders_height);
	WINDOW *w_files = create_window(_w_folders_width + 2, 4, _w_files_width, _w_files_height);
	WINDOW *w_hud = create_window(1, _w_folders_height + 4, _w_path_width, 5);
	WINDOW *windows[] = {w_path, w_folders, w_files, w_hud, 0};
	
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));

	struct fs_entry *folders[1024], *files[1024];
	malloc_entries(folders, 1024);
	malloc_entries(files, 1024);
	int num_folders, num_files;
	get_folders_and_files(cwd, folders, files, &num_folders, &num_files);
	
	bool first = true;
	int curr_window = FOLDERS;
	int selection = 0;
	int folder_selection = 0, file_selection = 0;
	int offset_folders = 0, offset_files = 0;
	int input;
	while (first || (input = getch()) != 'q') {
		first = false;

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
					next_dir(cwd, new_dir);
					get_folders_and_files(cwd, folders, files, &num_folders, &num_files);
					folder_selection = file_selection = 0;
					offset_folders = offset_files = 0;
					clear_windows(windows);
				} else {
					char cmd[1024] = "", command[1024] = "";
					if (files[file_selection]->can_exec) {
						sprintf(cmd, "%s/%s", cwd, 
							files[file_selection]->ent->d_name);
						escape_path(cmd, command, false);
					} else {
						sprintf(cmd, "vim %s/%s", cwd, 
							files[file_selection]->ent->d_name);
						escape_path(cmd, command, true);
					}
					run_command(command);
				}
				break;
			default:
				break;
		}

		wprint_str(w_path, 2, 1, cwd);
		show_entries(w_folders, FOLDERS, folders, folder_selection, offset_folders, 
				curr_window == FOLDERS, &show_folder);
		show_entries(w_files, FILES, files, file_selection, offset_files, 
				curr_window == FILES, &show_file);
		draw_scrollbar(w_folders, num_folders, _w_folders_width - 2,
				_w_folders_height - 2, offset_folders);
		draw_scrollbar(w_files, num_files, _w_files_width - 2,
				_w_files_height - 2, offset_files);
		draw_title(w_folders, "folders");
		draw_title(w_files, "files");
		struct fs_entry *selected_entry = curr_window ? 
			files[file_selection] : folders[folder_selection];
		draw_hud(w_hud, selected_entry); 
	
		wrefresh(w_path);
		wrefresh(w_folders);
		wrefresh(w_files);
		wrefresh(w_hud);
		refresh();
	}

	destroy_window(w_path);
	destroy_window(w_folders);
	destroy_window(w_files);
	destroy_window(w_hud);

	endwin();
	closelog();
	return 0;
}

void init_ncurses(void)
{
	initscr();
	cbreak();
	noecho();
	curs_set(0);
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
	_w_folders_height = LINES - 10;
	_w_files_width = 2 * COLS / 3 - 2;
	_w_files_height = _w_folders_height;
	_max_folders = _w_folders_height - 2;
	_max_files = _w_files_height - 2;
}

void show_entries(WINDOW *win, int win_type, struct fs_entry **entries, int selection, int offset, bool is_active, 
		void (*show)(WINDOW *win, int y, int width, struct fs_entry *entry, bool is_selected))
{
	int max_entries = win_type ? _max_files : _max_folders;
	int width = win_type ? _w_files_width - 23 : _w_folders_width - 3;
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
	char output[1024] = "";
	sprintf(output, "%-*.*s", width, width, folder->ent->d_name);
        wprint_str(win, 2, y, output);	
	wattroff(win, COLOR_PAIR(color));
}

void show_file(WINDOW *win, int y, int width, struct fs_entry *file, bool is_selected)
{
	int color = get_file_color(file);
	char permissions[4];
	get_permissions(file, permissions);
	char *name = file->ent->d_name;
	off_t size = file->stat->st_size;
	int type = file->ent->d_type;
	int name_width = width - 33; //width / 3 * 2;
	char date_str[128];
	get_date(file, date_str);
	wattron(win, COLOR_PAIR(color));
	char output[1024] = "";
	sprintf(output, "%-*.*s %11d %s [%s %2d]", name_width, name_width, name,
			(int)size, date_str, permissions, type);
	wprint_str(win, 2, y, output);
	wattroff(win, COLOR_PAIR(color));
}

int get_file_color(struct fs_entry *file)
{
	if (!file->can_read)
		return COLOR_BLUE;
	else if (file->ent->d_type != DT_REG)
		return COLOR_CYAN;
	else if (file->can_exec)
		return COLOR_GREEN;
	else
		return COLOR_WHITE;
}

void get_date(struct fs_entry *file, char *str)
{
	time_t now;
	time(&now);
	int curr_year = localtime(&now)->tm_year;
	time_t *f_time = &file->stat->st_mtime;
	struct tm *time_ptr = localtime(f_time);
	if(time_ptr->tm_year == curr_year)
		strftime(str, 13, "%b %d %H:%M", time_ptr);
	else
		strftime(str, 13, "%b %d  %Y", time_ptr);
}

void get_permissions(struct fs_entry *entry, char *str)
{
	str[0] = entry->can_read ? 'r' : ' ';
	str[1] = entry->can_write ? 'w' : ' ';
	str[2] = entry->can_exec ? 'x' : ' ';
	str[3] = '\0';
}

void draw_hud(WINDOW *win, struct fs_entry *entry)
{
	mode_t mode = entry->stat->st_mode;
	nlink_t nlink = entry->stat->st_nlink;
	uid_t user_id = entry->stat->st_uid;
	struct passwd *user = getpwuid(user_id);
	gid_t group_id = entry->stat->st_gid;
	off_t size = entry->stat->st_size;
	char date_str[128] = "";
	get_date(entry, date_str);
	char output[1024] = "";
	int n = sprintf(output, "%d %d %s %d %d %s %-100.100s", 
			mode, (int)nlink, user->pw_name, group_id, (int)size, date_str, entry->ent->d_name);
	wprint_str(win, 2, 1, output);
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
			wprint_chr(win, x, y, c);
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

