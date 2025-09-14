#include "config/gconfig.h"
#include "core/strategy.h"
#include "utils.h"
#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h> 
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MPT_VERSION "3 (DEV)"


FILE* template_file;

static void tmplt(char* l) {
	char* add_content = malloc(sizeof(char)*1);
	add_content[0] = 0;
	
	if (is_dir(l)) {	
		append_to_str(&add_content, "[");
		append_to_str(&add_content, l);
		append_to_str(&add_content, "]\ntype=\"dir\"\n\n");
		
		fputs(add_content, template_file);
	}
	else {
		append_to_str(&add_content, "[");
		append_to_str(&add_content, l);
		append_to_str(&add_content, "]\ntype=\"file\"\ncontent=\"");

		char* file_content = read_file(l);
		char* new_file_content = malloc(sizeof(char)*1);
		new_file_content[0] = 0;

		for(int i = 0; file_content[i] != 0; i++) {
			if (file_content[i] == '"') {
				char t[3];
				sprintf(t, "\\\"");
				append_to_str(&new_file_content, t);
			}
			else {
				char t[2];
				sprintf(t, "%c", file_content[i]);
				append_to_str(&new_file_content, t);
			}
		}

		append_to_str(&add_content, new_file_content);
		append_to_str(&add_content, "\"\n\n");

		fputs(add_content, template_file);
		free(new_file_content);
	}
	
	free(add_content);
}

void make_template(int argc, char** args) {
	if (argc != 2) {
		printf("mpt help for help\n");
		return;
	}
	
	char* template_name = args[0];
	char* template_dir = args[1];
	
	if ((template_file = fopen(template_name, "w")) == NULL) {
		fprintf(stderr, "*Failed to create/write to the file '%s'*\n", template_name);
		return;
	}

	if (!is_dir(template_dir)) {
		fprintf(stderr, "*%s is not a directory*\n", template_dir);
		return;
	}
	chdir(template_dir);

	list_dir(".", tmplt, true, true);

	fclose(template_file);
}

void help() {
	printf("\x1b[1mMPT\x1b[0m Usage: mpt [command] [args]\n Commands:\n");
	printf("%-34s - %s\n", "info", "Show current version");
	printf("%-34s - %s\n", "gen-cf [target]", "Generate 'compile_flags.txt' based on 'flags' and 'includes' fields");
	printf("%-34s - %s\n", "build [target]", "Build target");
	printf("%-34s - %s\n", "run [target]", "Build and run target");
	printf("%-34s - %s\n", "template [template_name] [dir]", "Make template based on directory");
	printf("%-34s - %s\n", "new [template] [project name]", "Make new project from template");
}

void new(int argc, char** args) {
	if (argc != 2) {
		printf("mpt help for help\n");
		return;
	}

	char* template_name = args[0];
	char* project_name = args[1];
	
	char template_location[PATH_MAX];
	template_location[0] = 0;
	
	// Get templates directory
	char* last = strrchr(self, '/');
	char* first = &self[0];

	char* new_buf = malloc(sizeof(char) * (strlen(self)));
	int i = 0;

	for (; first != last; first++) {
		new_buf[i] = *first;
		i++;
	}

	new_buf[i] = 0;

	strcat(template_location, new_buf);
	strcat(template_location, "/templates/");
	strcat(template_location, template_name);

	free(new_buf);
	
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

	printf("\x1b[1mMPT\x1b[0m\nProject '%s' initialized using template '%s'\n", project_name, template_name); fflush(stdout);
}

void get_current_target(char* desired) {
	for (int i = 0; i < GLOBAL_CONFIG.target_count; i++) {
		if(strcmp(desired, GLOBAL_CONFIG.targets[i]->name) == 0) {
			CURRENT_TARGET = *GLOBAL_CONFIG.targets[i];
			return;
		}
	}

	fprintf(stderr, "*Target '%s' not found*\n", desired);
	exit(3);
}

void generate_cf(int argc, char** args) {
	parse_global_cfg(read_file("Project"));

	if (argc == 0) {
		CURRENT_TARGET = *GLOBAL_CONFIG.targets[0];
	}
	else {
		get_current_target(args[0]);
	}

	FILE* file = fopen("compile_flags.txt", "w");
	if (!file) {
		fprintf(stderr, "*Failed to create '.clangd'*\n");
		exit(3);
	}

	char* cf_content = malloc(sizeof(char));
	cf_content[0] = 0;
	
	char buf[256];
	int b = 0;

	for (int i = 0; CURRENT_TARGET.includes[i] != 0; i++) {
		if (CURRENT_TARGET.includes[i] == ':' || CURRENT_TARGET.includes[i+1] == 0) {
			if (CURRENT_TARGET.includes[i+1] == 0) {
				buf[b] = CURRENT_TARGET.includes[i];
				b++;
			}
			buf[b] = 0;
			append_to_str(&cf_content, "-I");
			append_to_str(&cf_content, buf);
			append_to_str(&cf_content, "\n");
			b = 0;
			continue;
		}

		buf[b] = CURRENT_TARGET.includes[i];
		b++;
	}
	
	fprintf(file, "%s", cf_content);

	for (int i = 0; CURRENT_TARGET.flags[i] != 0; i++) {
		if (CURRENT_TARGET.flags[i] == ' ') {
			fprintf(file, "\n");
			continue;
		}

		fprintf(file, "%c", CURRENT_TARGET.flags[i]);
	}

	fclose(file);
}

void build(int argc, char** args) {
	init_strategies();
	parse_global_cfg(read_file("Project"));

	if (argc == 0) {
		CURRENT_TARGET = *GLOBAL_CONFIG.targets[0];	
	}
	else {
		get_current_target(args[0]);
	}

	build_with_target(CURRENT_TARGET);
}

void run(int argc, char** args) {
	build(argc, args);
	char* cmd = malloc(sizeof(char));
	cmd[0] = 0;
	append_to_str(&cmd, "./");
	append_to_str(&cmd, CURRENT_TARGET.binary_name);
	
	system(cmd);
	free(cmd);
}

int main(int argc, char** args) {
	if (argc == 0) return -90;
	realpath(args[0], self);

	if (strcmp(args[argc-1], "--silent") == 0) {
		silent = true;
		argc--;
	}

	if (argc == 1) {
		printf("%s help for help\n", args[0]);
		return 0;
	}

	if (strcmp(args[1], "help") == 0) {
		help();
		return 0;
	}

	if (strcmp(args[1], "build") == 0) {
		build(argc-2, args+2);
		return 0;
	}

	if (strcmp(args[1], "run") == 0) {
		run(argc-2, args+2);
		return 0;
	}

	if (strcmp(args[1], "info") == 0 ||
	    strcmp(args[1], "version") == 0) {
		printf("MPT v" MPT_VERSION "\n");
		return 0;
	}

	if (strcmp(args[1], "new") == 0) {
		new(argc-2, args+2);
		return 0;
	}

	if (strcmp(args[1], "gen-cf") == 0) {
		generate_cf(argc-2, args+2);
		return 0;
	}

	if (strcmp(args[1], "template") == 0) {
		make_template(argc-2, args+2);
		return 0;
	}
	
	if (!silent)
		printf("%s help for help\n", args[0]);	

	return 0;
}
