#include "config/gconfig.h"
#include "core/strategy.h"
#include "core/strategy_impl/c_console.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h> 
#include <stdio.h>

void help() {
	// TODO: Help someone
}

void build(int argc, char** args) {
	init_strategies();
	parse_cfg(read_file("Project"));

	if (argc == 0) {
		CURRENT_TARGET = *GLOBAL_CONFIG.targets[0];	
	}
	else {
		char* desired = args[0];

		for (int i = 0; i < GLOBAL_CONFIG.target_count; i++) {
			if(strcmp(desired, GLOBAL_CONFIG.targets[i]->name) == 0) {
				CURRENT_TARGET = *GLOBAL_CONFIG.targets[i];
				goto success;
			}
		}

		printf("*Target '%s' not found*\n", desired);
		exit(3);
	}
success:

	if (strcmp(GLOBAL_CONFIG.strategy, "c_console") == 0) {
		do_strategy(c_console);
		return;
	}

	if (strcmp(GLOBAL_CONFIG.strategy, "dummy") == 0) {
		do_strategy(dummy);
		return;
	}

	printf("Unknown strategy: %s\n", GLOBAL_CONFIG.strategy);
}

int main(int argc, char** args) {
	if (argc <= 1) {
		help();
		return 0;
	}

	if (strcmp(args[1], "build") == 0) {
		build(argc-2, args+2);
		return 0;
	}

	if (strcmp(args[1], "info") == 0) {
		printf("MPT v0\n");
		return 0;
	}

	if (strcmp(args[1], "new") == 0) {
		// TODO: add template functionality
		return 0;
	}

	help();

	return 0;
}
