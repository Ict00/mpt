#pragma once

#include <stddef.h>

struct {
	char* name;
	char* content;
} typedef td_file;

struct {
	char* dirs;
	td_file* files;
	size_t file_count;
} typedef td_project;
