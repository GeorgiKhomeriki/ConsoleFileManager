#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>

void wprint_chr(WINDOW* win, int x, int y, char c);
void wprint_str(WINDOW* win, int x, int y, char *s);
int last_index(char* s, char c);
char *escape_path(char *str, char *new_str, bool skip_first);
void next_dir(char *path, char *new_dir, int str_len);

