#include <ncurses.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include "window.h"

void show_folders(WINDOW *w_folders, struct dirent **folders);
void show_files(WINDOW *w_files, struct dirent **files);
void get_folders_and_files(char *path, struct dirent **folder, struct dirent **files);
void read_entries(char *path, struct dirent **entries);
void split_entries(struct dirent **entries, struct dirent **folders, struct dirent **files);
