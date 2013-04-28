#include "reader.h"

void get_folders_and_files(char *path, struct fs_entry **folders, struct fs_entry **files, int *num_folders, int *num_files)
{
	struct dirent *entries[1024];
	read_entries(path, entries);
	split_entries(entries, path, folders, files, num_folders, num_files);
}

void read_entries(char *path, struct dirent **entries)
{
	DIR *dir = opendir(path);
	if (dir != NULL) {
		int i;
		struct dirent *entry;
		for (i = 0; (entry = readdir(dir)) != NULL; i++)
			entries[i] = entry;
		entries[i] = NULL;
		closedir(dir);
	}
}

void split_entries(struct dirent **entries, char *path, struct fs_entry **folders, struct fs_entry **files, int *num_folders, int *num_files)
{
	int i, folder_i, file_i;
	folder_i = file_i = 0;
	struct dirent *ent;
	for (i = 0; (ent = entries[i]) != NULL; i++) {
		if (is_sane(ent)) {
			struct fs_entry *entry = ent->d_type == DT_DIR ?
				folders[folder_i++] : files[file_i++];
			entry->ent = ent;
			populate_entry(entry, path);
		}
	}
	folders[folder_i]->ent = files[file_i]->ent = NULL;
	*num_folders = folder_i;
	*num_files = file_i;
	qsort(folders, folder_i, sizeof(struct fs_entry *), compare_entries);
	qsort(files, file_i, sizeof(struct fs_entry *), compare_entries);
}

bool is_sane(struct dirent *entry)
{
	int type = entry->d_type;
	return type >= 0 && type <= 14 && !(type % 2);
}

void populate_entry(struct fs_entry *entry, char *path)
{
	char full_path[1024];
	sprintf(full_path, "%s/%s", path, entry->ent->d_name);
	if (stat(full_path, entry->stat))
		syslog(LOG_NOTICE, "error: %s ", full_path);
	entry->can_read = !access(full_path, R_OK);
	entry->can_write = !access(full_path, W_OK);
	entry->can_exec = !access(full_path, X_OK);
}

void malloc_entries(struct fs_entry **entries, int num_entries)
{
	int i;
	for (i = 0; i < num_entries; i++) {
		entries[i] = malloc(sizeof(struct fs_entry));
		entries[i]->stat = malloc(sizeof(struct stat));
	}
}

static int compare_entries(const void *p1, const void *p2)
{
	struct fs_entry *e1 = *(struct fs_entry * const *)p1;
	struct fs_entry *e2 = *(struct fs_entry * const *)p2;
	return strcmp(e1->ent->d_name, e2->ent->d_name);
}

