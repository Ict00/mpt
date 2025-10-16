#include "common.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include "../../config/gconfig.h"
#include "../../utils.h"

void common_done() {
	system(CURRENT_TARGET.post_cmd);
}

void common_start() {
	system(CURRENT_TARGET.pre_cmd);
	char path[256];
	int b = 0;
	
	for (int i = 0; CURRENT_TARGET.makeDirs[i] != 0; i++) {
		if (CURRENT_TARGET.makeDirs[i] == ':' || CURRENT_TARGET.makeDirs[i+1] == 0) {
			if (CURRENT_TARGET.makeDirs[i+1] == 0) {
				path[b] = CURRENT_TARGET.makeDirs[i];
				b++;
			}
			
			path[b] = 0;
			b = 0;
			
			if (!is_dir(path)) {
				mkdir(path, 0775);
			}
			
			continue;
		}
		
		path[b] = CURRENT_TARGET.makeDirs[i];
		b++;
	}
}

void build_subprojects() {
	char original_path[PATH_MAX];

	if (getcwd(original_path, PATH_MAX) != NULL) {
		char path[256];
		int b = 0;

		for (int i = 0; CURRENT_TARGET.subprojects[i] != 0; i++) {
			if (CURRENT_TARGET.subprojects[i] == ':' || CURRENT_TARGET.subprojects[i+1] == 0) {
				if (CURRENT_TARGET.subprojects[i+1] == 0) {
					path[b] = CURRENT_TARGET.subprojects[i];
					b++;
				}
				
				path[b] = 0;
				b = 0;
				
				if (!is_dir(path)) {
					fprintf(stderr, "*Failed to build subprojects: Path '%s' is not a directory/doesn't exist*\n", path);
					exit(-1);
				}
				
				chdir(path);

				char cmd[1024];
				sprintf(cmd, "%s build --silent", self);
				
				if(system(cmd) != 0) {
					chdir(original_path);
					exit(-1);
				}

				chdir(original_path);

				continue;
			}

			path[b] = CURRENT_TARGET.subprojects[i];
			b++;
		}
	}
	else {
		fprintf(stderr, "*Failed to build subprojects: Unable to get cwd*\n");
		exit(-1);
	}
}

void common_includes() {
	char path[256];

	int b = 0;

	for (int i = 0; CURRENT_TARGET.includes[i] != 0; i++) {
		if (CURRENT_TARGET.includes[i] == ':' || CURRENT_TARGET.includes[i+1] == 0) {
			if (CURRENT_TARGET.includes[i+1] == 0) {
				path[b] = CURRENT_TARGET.includes[i];
				b++;
			}

			path[b] = 0;

			append_to_str(&CURRENT_TARGET.flags, " -I");
			append_to_str(&CURRENT_TARGET.flags, path);

			b = 0;

			continue;
		}

		path[b] = CURRENT_TARGET.includes[i];
		b++;
	}
}
