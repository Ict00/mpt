#pragma once

#include <stdbool.h> 

#define PROJ_NAME "MPT"
#define is_dir(expr) !is_file(#expr)

typedef void (*listdir_exec)(char* path);

char* get_file_name(char* path);
bool ends_with(char* src, char* what);
bool is_file(const char* path);
char* read_file(const char* file_name);
void list_dir(char* path, listdir_exec function, bool recursive);
char* bar(int length, int max_amount, int amount);
void out_status(const char* msg, char* bar, int max, int amount);
