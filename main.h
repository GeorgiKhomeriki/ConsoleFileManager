#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include "window.h"
#include "reader.h"
#include "util.h"

#define FOLDERS		0
#define FILES		1

#define UP		-1
#define DOWN		1

static int _w_path_width;
static int _w_folders_width;
static int _w_folders_height;
static int _w_files_width;
static int _w_files_height;
static int _max_folders;
static int _max_files;

void next_dir(char *path, char *new_dir);
void init_ncurses(void);
void init_colors(void);
void init_screen_params(void);
void lock_fps(clock_t start, int fps);
void show_entries(WINDOW *win, int win_type, struct fs_entry **entries, int selection, int offset, bool is_active,
		void (*show)(WINDOW *win, int y, int width, struct fs_entry *entry, bool is_selected));
void show_folder(WINDOW *win, int y, int width, struct fs_entry *entry, bool is_selected);
void show_file(WINDOW *win, int y, int width, struct fs_entry *file, bool is_selected);
int get_file_color(struct fs_entry *file);
void get_date(struct fs_entry *file, char *str);
void get_permissions(struct fs_entry *entry, char *str);
void draw_scrollbar(WINDOW *win, int num_entries, int x, int win_height, int offset);
void move_cursor(WINDOW *win, int curr_window, int *selection, int num_entries, int *offset, int dir);
void scroll_window(WINDOW *win, int curr_window, int *selection, int *offset, int delta);
void run_command(char *command);

