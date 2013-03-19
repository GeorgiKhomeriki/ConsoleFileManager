#include <ncurses.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "window.h"
#include "reader.h"
#include "util.h"

#define FOLDERS		0
#define FILES		1

static int _w_path_width;
static int _w_folders_width;
static int _w_folders_height;
static int _w_files_width;
static int _w_files_height;

void init_screen_params();
void lock_fps(clock_t start, int fps);
void show_folders(WINDOW *w_folders, struct dirent **folders, int selection, bool is_active);
void show_files(WINDOW *w_files, struct dirent **files, int selection, bool is_active);

