#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

struct fs_entry {
	struct dirent *ent;
};

void get_folders_and_files(char *path, struct fs_entry **folders, struct fs_entry **files, int *num_folders, int *num_files );
void read_entries(char *path, struct dirent **entries);
void split_entries(struct dirent **entries, struct fs_entry **folders, struct fs_entry **files, int *num_folders, int *num_files);
void malloc_entries(struct fs_entry **entries, int num_entries);

