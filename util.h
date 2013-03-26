#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int last_index(char* s, char c);
char *escape_path(char *str, char *new_str, bool skip_first);
void next_dir(char *path, char *new_dir, int str_len);

