#include "util.h"

void wprint_chr(WINDOW* win, int x, int y, char c)
{
	wmove(win, y, x);
	waddch(win, c);
}

void wprint_str(WINDOW* win, int x, int y, char *s)
{
	wmove(win, y, x);
	int i;
	for (i = 0; s[i]; i++)
		waddch(win, s[i]);
}

int last_index(char* s, char c)
{
	int i, result = 0;
	for(i = 0; s[i]; i++)
		if(s[i] == c)
			result = i;
	return result;
}

char *escape_path(char *str, char *new_str, bool skip_first)
{
	int i, j;
	int first = skip_first ? strstr(str, " ") - str : -1;
	for (i = 0, j = 0; str[i]; i++, j++) {
		if (i > first && str[i] == ' ')
			new_str[j++] = '\\';
		new_str[j] = str[i];
	}
}

void next_dir(char *path, char *new_dir, int str_len)
{
	if (!strcmp(new_dir, "..")) {
		int slash_index = last_index(path, '/');
		if(!slash_index)
			snprintf(path, str_len, "/");
		else
			path[slash_index] = '\0';
	} else if (strcmp(new_dir, ".")) {
		if(!strcmp(path, "/"))
			path[0] = '\0';
		snprintf(path, str_len, "%s/%s", path, new_dir);
	}
}

