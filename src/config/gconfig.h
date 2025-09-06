#pragma once

#include <stddef.h>
#include "../core/template.h"

struct {
	char* compiler;
	char* flags;
	char* ldflags;
	char* binary_name;
	char* post_cmd;
	char* name;
	char* sources;
} typedef target;

struct {
	target** targets;
	size_t target_count;
	char* strategy;
} typedef global_config;

extern target CURRENT_TARGET;
extern global_config GLOBAL_CONFIG;

void parse_cfg(char* config_text);
td_project parse_tcfg(char* config_text);
