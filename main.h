#include <ncurses.h>
#include "window.h"
#include "reader.h"

void show_folders(WINDOW *w_folders, struct dirent **folders, int selected);
void show_files(WINDOW *w_files, struct dirent **files);

