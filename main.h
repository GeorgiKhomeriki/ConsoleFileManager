#include <ncurses.h>
#include <time.h>
#include <string.h>
#include "window.h"
#include "reader.h"

#define FOLDERS		0
#define FILES		1

void lock_fps(clock_t start, int fps);
void show_folders(WINDOW *w_folders, struct dirent **folders, int selection, bool is_active);
void show_files(WINDOW *w_files, struct dirent **files, int selection, bool is_active);

