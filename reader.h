#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

void get_folders_and_files(char *path, struct dirent **folder, struct dirent **files, int *num_folders, int *num_files );
void read_entries(char *path, struct dirent **entries);
void split_entries(struct dirent **entries, struct dirent **folders, struct dirent **files, int *num_folders, int *num_files);
