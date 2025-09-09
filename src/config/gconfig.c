#include "gconfig.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>

target CURRENT_TARGET;
build_config GLOBAL_CONFIG;

static bool is_space(char ch) {
	return !(ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r' && ch != ' ');
}

static void skip_spaces(char* text, int* pos) {
	for (;is_space(text[*pos]);(*pos)++);
}

static char* parse_str(char* text, int* pos) {
	int i = 0;
	char buf[256];

	(*pos)++;

	skip_spaces(text, pos);
	
	for(;text[*pos] != 0 && text[*pos] != '"'; (*pos)++) {
		if (text[*pos] == '\\') {
			if (text[(*pos)+1] == '"') {
				buf[i] = '"';
				i++;
				(*pos)++;
				continue;
			}
		}

		buf[i] = text[*pos];
		i++;
	}

	buf[i] = 0;

	return strdup(buf);
}

static char* parse_var(char* text, char** var_name, int* pos) {
	char var_name_buf[64];
	char var_val_buf[256];
	int i = 0;
	
	for(; text[*pos] != 0 && text[*pos] != '='; (*pos)++) {
		if (!is_space(text[*pos])) {
			var_name_buf[i] = text[*pos];
			i++;
		}
	}
	var_name_buf[i] = 0;

	*var_name = strdup(var_name_buf);
	
	if (text[*pos] == 0) {
		fprintf(stderr, "ERR WHILE PARSING\nExpected value, got EOF\nPosition: %d\n", *pos);
		exit(2);
	}
	
	(*pos)++;

	skip_spaces(text, pos);
	
	if(text[*pos] != '"') {
		fprintf(stderr, "ERR WHILE PARSING\nExpected value, got '%c'\nPosition: %d\n", text[*pos], *pos);
		exit(2);
	}

	char* res = parse_str(text, pos);
	strcpy(var_val_buf, res);	
	free(res);	

	return strdup(var_val_buf);
}

static void assign_to_target(target* dest, char* fname, char* fvalue) {
	if (strcmp(fname, "compiler") == 0) {
		free(dest->compiler);
		dest->compiler = fvalue;
		return;
	}
	if (strcmp(fname, "post_cmd") == 0 || strcmp(fname, "cmd") == 0) {
		free(dest->post_cmd);
		dest->post_cmd = fvalue;
		return;
	}
	if (strcmp(fname, "pre_cmd") == 0 || strcmp(fname, "init_cmd") == 0) {
		free(dest->pre_cmd);
		dest->pre_cmd = fvalue;
		return;
	}
	if (strcmp(fname, "binary_name") == 0 || strcmp(fname, "output") == 0) {
		free(dest->binary_name);
		dest->binary_name = fvalue;
		return;
	}
	if (strcmp(fname, "ldflags") == 0) {
		free(dest->ldflags);
		dest->ldflags = fvalue;
		return;
	}
	if (strcmp(fname, "sources") == 0) {
		free(dest->sources);
		dest->sources = fvalue;
		return;
	}
	if (strcmp(fname, "flags") == 0) {
		free(dest->flags);
		dest->flags = fvalue;
		return;
	}
	fprintf(stderr, "ERR WHILE PARSING\nInvalid config var: %s\n", fname);
	exit(3);
}

static char* parse_head(char* text, int* pos) {
	char target_name[32];
	int i = 0;
	for(;text[*pos] != 0 && text[*pos] != '[' && text[*pos] != ']'; (*pos)++);
	
	if (text[*pos] == ']') {
		goto expected_head;
	}

	if (text[*pos] == 0) {
expected_head:
		fprintf(stderr, "ERR WHILE PARSING\nExpected [head]\nPosition: %d\n", *pos);
		exit(2);
	}

	(*pos)++;

	for (;text[*pos] != 0 && text[*pos] != ']'; (*pos)++) {
		if (!is_space(text[*pos])) {
			target_name[i] = text[*pos];
			i++;
		}
	}
	target_name[i] = 0;
	
	if (text[*pos] == 0) {
		fprintf(stderr, "ERR WHILE PARSING\nUnclosed '['\nPosition: %d\n", *pos);
		exit(2);
	}

	return strdup(target_name);
}

static target* parse_target(char* text, int* pos) {
	target* res = malloc(sizeof(target));
	
	res->ldflags = strdup("-O3");
	res->flags = strdup("-O3 -std=c23 -c");
	res->post_cmd = strdup("");
	res->pre_cmd = strdup("");
	res->compiler = strdup("cc");
	res->binary_name = strdup("program");
	res->sources = strdup("src");

	char cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		free(res->binary_name);
		res->binary_name = strdup(basename(cwd));
	}

	res->name = parse_head(text, pos);

	(*pos)++;

	skip_spaces(text, pos);

	for (;text[*pos] != 0 && text[*pos] != '['; (*pos)++) {
		if (!is_space(text[*pos])) {
			char* varname;
			char* varval = parse_var(text, &varname, pos);
			assign_to_target(res, varname, varval);
		}
	}
	
	return res;
}

void parse_global_cfg(char *config_text) {
	GLOBAL_CONFIG = parse_cfg(config_text);
}

build_config parse_cfg(char *config_text) {
	int i = 0;
	char* EMPTY;

	char* strategy = parse_var(config_text, &EMPTY, &i);

	if (strcmp(EMPTY, "strategy") != 0) {
		fprintf(stderr, "ERR WHILE PARSING\nExpected 'strategy', got '%s'\nPosition: %d\n", EMPTY, i);
		exit(2);
	}

	skip_spaces(config_text, &i);
	
	target** targets = malloc(sizeof(target*));
	size_t target_count = 1;
	
	for (; config_text[i] != 0;) {
		target* t = parse_target(config_text, &i);
		targets = realloc(targets, sizeof(target*) * (target_count + 1));
		targets[target_count-1] = t;
		target_count++;
	}
	targets = realloc(targets, sizeof(target*) * (target_count - 1));
	target_count--;

	free(EMPTY);
	
	return (build_config){
		.targets = targets,
		.target_count = target_count,
		.strategy = strategy
	};
}

td_project parse_tcfg(char *config_text) {
	td_project project = {.file_count=1};
	td_file** files = malloc(sizeof(td_file*));

	int pos = 0;
	char dirs[4096];
	dirs[0] = 0;
	
	for (; config_text[pos] != 0;) {
		skip_spaces(config_text, &pos);
		if (config_text[pos] == 0) break;

		char* head = parse_head(config_text, &pos);
		pos++;
		skip_spaces(config_text, &pos);

		char* EMPTY;
		char* type = parse_var(config_text, &EMPTY, &pos);
		pos++;

		if (strcmp(EMPTY, "type") != 0) {
			fprintf(stderr, "ERR WHILE PARSING\nExpected type declaration\nPosition: %d\n", pos);
			exit(2);
		}
		
		if (strcmp(type, "dir") == 0) {
			strcat(dirs, head);
			free(head);
			strcat(dirs, ":");
		}
		else if (strcmp(type, "file") == 0) {
			free(EMPTY);
			skip_spaces(config_text, &pos);
			char* content = parse_var(config_text, &EMPTY, &pos);
			pos++;
			
			if (strcmp(EMPTY, "content") != 0) {
				fprintf(stderr, "ERR WHILE PARSING\nExpected content declaration\nPosition: %d\n", pos);
				exit(2);
			}
			
			td_file* file = malloc(sizeof(td_file));

			file->name = head;
			file->content = content;

			files = realloc(files, sizeof(td_file*)*(project.file_count+1));
			files[project.file_count-1] = file;
			project.file_count++;

			free(EMPTY);
		}
		else {
			fprintf(stderr, "ERR WHILE PARSING\nInvalid type in type declaration: %s\nPosition: %d\n", type, pos);
			exit(2);
		}
	}
	
	files = realloc(files, sizeof(td_file*) * (project.file_count - 1));
	
	project.files = files;
	project.file_count--;

	if (strlen(dirs) > 0)
		dirs[strlen(dirs)-1] = 0;
	
	project.dirs = strdup(dirs);

	return project;
}
