#include "reader.h"

void get_folders_and_files(char *path, struct fs_entry **folders, struct fs_entry **files, int *num_folders, int *num_files)
{
	struct dirent *entries[1024];
	read_entries(path, entries);
	split_entries(entries, folders, files, num_folders, num_files);
}

void read_entries(char *path, struct dirent **entries)
{
	DIR *dir = opendir(path);
	if(dir != NULL) {
		int i;
		struct dirent *entry;
		for(i = 0; (entry = readdir(dir)) != NULL; i++)
			entries[i] = entry;
		entries[i] = NULL;
		closedir(dir);
	}
}

void split_entries(struct dirent **entries, struct fs_entry **folders, struct fs_entry **files, int *num_folders, int *num_files)
{
	int i, folder_i, file_i;
	folder_i = file_i = 0;
	struct dirent *entry;
	for(i = 0; (entry = entries[i]) != NULL; i++) {
		if(entry->d_type == DT_DIR)
			folders[folder_i++]->ent = entry;
		else
			files[file_i++]->ent = entry;
	}
	folders[folder_i]->ent = files[file_i]->ent = NULL;
	*num_folders = folder_i;
	*num_files = file_i;
}

void malloc_entries(struct fs_entry **entries, int num_entries)
{
	int i;
	for (i = 0; i < num_entries; i++) 
		entries[i] = malloc(sizeof(struct fs_entry));
}

