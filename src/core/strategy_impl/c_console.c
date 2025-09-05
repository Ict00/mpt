#include "c_console.h"
#include "string.h"
#include "../../utils.h"
#include "../../config/gconfig.h"
#include <linux/limits.h>
#include <stdlib.h>
#include <stdio.h>

int compiled = 0;

void compile_sf(char *l) {
	if (!ends_with(l, ".c"))
		return;

	char cmd[1024];
	snprintf(cmd, sizeof(cmd), "%s %s -o obj/%d.o %s", CURRENT_TARGET.compiler, l, compiled, CURRENT_TARGET.flags);

	int i = system(cmd);

	if (i != 0) {
		printf("*Error occured during compilation*\n");
		exit(i);
	}

	compiled++;
}

void cct_done() {
	system(CURRENT_TARGET.post_cmd);
}

void cct_compile() {
	char buffer[PATH_MAX];
	int bi = 0;
	int i = 0;
	for (; CURRENT_TARGET.sources[i] != 0; i++) {
		if (CURRENT_TARGET.sources[i] == ':') {
			buffer[bi] = 0;
			list_dir(buffer, compile_sf, true);
			bi = 0;
			continue;
		}

		buffer[bi] = CURRENT_TARGET.sources[i];
		bi++;
	}
	if (CURRENT_TARGET.sources[i] == 0) {
		buffer[bi] = 0;
		list_dir(buffer, compile_sf, true);
	}
}

void cct_link() {
	char to_link[4096] = {0};
	sprintf(to_link, "%s -o %s obj/*.o", CURRENT_TARGET.compiler, CURRENT_TARGET.binary_name);
	strcat(to_link, " ");
	strcat(to_link, CURRENT_TARGET.ldflags);
	int i = system(to_link);
	
	if (i != 0) {
		printf("*Error occured during linking*\n");
		exit(i);
	}
}
