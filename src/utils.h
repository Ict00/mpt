#pragma once

#include <stdbool.h> 

typedef void (*listdir_exec)(char* path);

void append_to_str(char** dest, const char* str);
bool ends_with(char* src, char* what);
bool is_file(const char* path);
bool is_dir(const char* path);
char* read_file(const char* file_name);
void list_dir(char* path, listdir_exec function, bool recursive, bool use_on_dirs);
char* bar(int length, int max_amount, int amount);
void out_status(const char* msg, char* bar, int max, int amount);
