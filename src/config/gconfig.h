#pragma once

#include <stddef.h>
#include "../core/template.h"

struct {
	char* compiler;
	char* flags;
	char* ldflags;
	char* binary_name;
	char* post_cmd;
	char* pre_cmd;
	char* includes;
	char* name;
	char* sources;
} typedef target;

struct {
	target** targets;
	size_t target_count;
	char* strategy;
} typedef build_config;

extern target CURRENT_TARGET;
extern build_config GLOBAL_CONFIG;

build_config parse_cfg(char* config_text);
void parse_global_cfg(char* config_text);
td_project parse_tcfg(char* config_text);
