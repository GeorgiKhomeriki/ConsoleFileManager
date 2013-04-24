#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <syslog.h>

struct fs_entry {
	struct dirent *ent;
	struct stat *stat;
	bool can_read;
	bool can_write;
	bool can_exec;
};

void get_folders_and_files(char *path, struct fs_entry **folders, struct fs_entry **files, int *num_folders, int *num_files );
void read_entries(char *path, struct dirent **entries);
void split_entries(struct dirent **entries, char *path, struct fs_entry **folders, struct fs_entry **files, int *num_folders, int *num_files);
bool is_sane(struct dirent *entry);
void populate_entry(struct fs_entry *entry, char *path);
void malloc_entries(struct fs_entry **entries, int num_entries);
static int compare_entries(const void *p1, const void *p2);

