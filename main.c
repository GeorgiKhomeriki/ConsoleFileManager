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

	struct dirent *entries[1024];
	read_entries(cwd, entries);	
	int i;
	for(i = 0; entries[i] != NULL; i++)
		mvwprintw(w_folders, i+1, 2, "%s", entries[i]->d_name);

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

void read_entries(char *path, struct dirent **entries)
{
	DIR *dir = opendir(path);
	if(dir != NULL) {
		int i;
		struct dirent *entry;
		for(i = 0; (entry = readdir(dir)) != NULL; i++)
			entries[i] = entry;
		closedir(dir);
	}
}

void split_entries(struct dirent **entries, struct dirent **folders, struct dirent **files)
{
	int i, folder_i, file_i;
	folder_i = file_i = 0;
	struct dirent *entry;
	for(i = 0; (entry = entries[i]) != NULL; i++) {
		if(entry->d_type == DT_DIR) {
			folders[folder_i] = entry;
			folder_i++;
		} else {
			files[file_i] = entry;
			file_i++;
		}
	}
}

