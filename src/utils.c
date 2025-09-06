#include "utils.h"
#include <linux/limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>
#include <unistd.h>

char* get_file_name(char* path) {
	char* filename_t = malloc(sizeof(char) * 255);
	int b = 0;

	for (int i = strlen(path)-1; path[i] != '/' && i >= 0; i--) {
		filename_t[b] = path[i];
		b++;
	}

	filename_t[b] = 0;
	
	char* filename = malloc(sizeof(char) * b);
	int _b = b-1;

	for (int i = 0; i < b; i++) {
		filename[i] = filename_t[_b];
		_b--;
	}
	
	filename[b] = 0;

	free(filename_t);

	return filename;
}

bool ends_with(char *src, char *what) {
	int b = strlen(what);

	for (int i = strlen(src); i >= 0 && b >= 0; i--) {
		if (what[b] != src[i])
			return false;
		b--;
	}

	return true;
}

char* read_file(const char *file_name) {
	FILE* file = fopen(file_name, "r");

	if (!file) {
		fprintf(stderr, "Failed to open file '%s'\n", file_name);
		exit(-1);
		return "";
	}

	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* content = malloc(length + 1);

	if (!content) {
		fclose(file);
		return "";
	}
	
	fread(content, 1, length, file);
	content[length] = 0;
	
	fclose(file);

	return content;
}

void list_dir(char *path, listdir_exec function, bool recursive) {
	DIR *dir;
	struct dirent *entry;

	dir = opendir(path);
	if (dir == NULL) {
		perror("opendir");
		return;
	}

	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		
		char npath[PATH_MAX];
		npath[0] = 0;
		strcat(npath, path);
		strcat(npath, "/");
		strcat(npath, entry->d_name);

		if (recursive) {
			if (!is_file(npath)) {
				list_dir(npath, function, recursive);
			}
			else {
				goto treat_as_file;
			}
		}
		else if(is_file(npath)) {
			goto treat_as_file;
		}

		continue;
treat_as_file:
		function(npath);
		continue;
	}
	
	closedir(dir);
}

bool is_file(const char *path) {
	struct stat path_stat;
	
	if (stat(path, &path_stat) != 0) {
		return false;
	}
	return S_ISREG(path_stat.st_mode);
}

bool is_dir(const char *path) {
	struct stat path_stat;
	
	if (stat(path, &path_stat) != 0) {
		return false;
	}
	return S_ISDIR(path_stat.st_mode);
}

char* bar(int length, int max_amount, int amount) {
	if (length <= 0 || amount < 0 || max_amount < amount || max_amount <= 0)
		return NULL;
	
	char* result = malloc((length+3)*sizeof(char));
	int i = 0;
	result[i] = '[';
	i++;

	int _a = (((float)amount) / ((float)max_amount)) * length;;
	int _l = length - _a;

	for (int b = 0; b < _a; b++) {
		result[i] = '#';
		i++;
	}

	for (int b = 0; b < _l; b++) {
		result[i] = '-';
		i++;
	}

	result[i-1] = ']';
	result[i] = 0;

	return result;
}

void out_status(const char *msg, char *bar, int max, int amount) {
	printf("\x1b[3A\x1b[200D");
	printf("\x1b[K%s\n", msg);
	printf("\x1b[K%s\n", bar);
	free(bar);
	printf("\x1b[KStep %d out of %d\n", amount, max);
	fflush(stdout);
}
