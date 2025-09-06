#include "config/gconfig.h"
#include "core/strategy.h"
#include "utils.h"
#include <errno.h>
#include <libgen.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h> 
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

void help() {
	// TODO: Help someone
	printf("\x1b[1mMPT\x1b[0m Usage:\nmpt build (target) - build target\n");
}

void new(int argc, char** args) {
	if (argc != 2) {
		help();
		return;
	}

	char* template_name = args[0];
	char* project_name = args[1];

	char* home_dir;

	if ((home_dir = getenv("HOME")) == NULL) {
		fprintf(stderr, "*HOME environment variable is not set*\n");
		exit(3);
	}

	char template_location[PATH_MAX];
	template_location[0] = 0;
	
	strcat(template_location, home_dir);
	strcat(template_location, "/.config/mpt/");
	strcat(template_location, template_name);
	
	if (!is_file(template_location) || is_dir(template_location)) {
		fprintf(stderr, "*Path '%s' is not a file/doesn't exist*\n", template_location);
		exit(3);
	}

	if (!is_dir(project_name) && !is_file(project_name)) {		
		// Hope nothing breaks x1
		if(mkdir(project_name, 0775) != 0 && errno != EEXIST) {
			perror("mkdir");
			exit(3);
		}
		chdir(project_name);
	}
	else {
		fprintf(stderr, "*Directory/file '%s' already exist*\n", project_name);
		exit(3);
	}

	char* config = read_file(template_location);
	td_project project = parse_tcfg(config);
	
	int b = 0;
	char buf[PATH_MAX];

	for (int i = 0; project.dirs[i] != 0; i++) {
		if (project.dirs[i] == ':' || project.dirs[i+1] == 0) {
			if (project.dirs[i+1] == 0) {
				buf[b] = project.dirs[i];
				b++;
			}

			buf[b] = 0;
			
			// Hope nothing breaks x2
			if (!is_dir(buf) && !is_file(buf))
				if(mkdir(buf, 0775) != 0 && errno != EEXIST) {
					perror("mkdir");
					exit(3);
				}

			b = 0;
			continue;
		}
		buf[b] = project.dirs[i];
		b++;
	}

	for (int i = 0; i < project.file_count; i++) {
		FILE* fptr = fopen(project.files[i]->name, "w");

		if (fptr == NULL) {
			fprintf(stderr, "*Error occured while creating file '%s'*\n", project.files[i]->name);
			exit(4);
		}

		fprintf(fptr, "%s", project.files[i]->content);

		fclose(fptr);
	}

	printf("Initialized!\n");
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

		fprintf(stderr, "*Target '%s' not found*\n", desired);
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

	fprintf(stderr, "Unknown strategy: %s\n", GLOBAL_CONFIG.strategy);
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
		new(argc-2, args+2);
		return 0;
	}

	help();

	return 0;
}
