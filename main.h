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
static int _max_folders;
static int _max_files;

void init_ncurses(void);
void init_screen_params(void);
void lock_fps(clock_t start, int fps);
void show_entities(WINDOW *win, int win_type, struct dirent **entities, int selection, int offset, bool is_active);
void scroll_window(WINDOW *win, int curr_window, int *selection, int *offset, int delta);

