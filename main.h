#include <ncurses.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include "window.h"

void read_entries(char *path, struct dirent **entries);
