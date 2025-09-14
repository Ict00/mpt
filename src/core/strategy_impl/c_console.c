#include "c_console.h"
#include "../../utils.h"
#include "../../config/gconfig.h"
#include <stdlib.h>
#include <stdio.h>

int compiled = 0;

void compile_sf(char *l) {
	if (!ends_with(l, ".c"))
		return;

	char cmd[1024];
	snprintf(cmd, sizeof(cmd), "%s %s -o obj/%d.o %s", CURRENT_TARGET.compiler, l, compiled, CURRENT_TARGET.flags);

	int status = system(cmd);

	if (status != 0) {
		fprintf(stderr, "*Error occured during compilation; status code: %d*\n", status);
		exit(status);
	}

	compiled++;
}

void cct_compile() {
	char buffer[PATH_MAX];
	int bi = 0;
	int i = 0;
	for (; CURRENT_TARGET.sources[i] != 0; i++) {
		if (CURRENT_TARGET.sources[i] == ':') {
			buffer[bi] = 0;
			list_dir(buffer, compile_sf, true, false);
			bi = 0;
			continue;
		}

		buffer[bi] = CURRENT_TARGET.sources[i];
		bi++;
	}

	if (CURRENT_TARGET.sources[i] == 0) {
		buffer[bi] = 0;
		list_dir(buffer, compile_sf, true, false);
	}
}

void cct_link() {
	char to_link[4096] = {0};
	sprintf(to_link, "%s -o %s obj/* %s", CURRENT_TARGET.compiler, CURRENT_TARGET.binary_name, CURRENT_TARGET.ldflags);
	
	int status = system(to_link);

	if (status != 0) {
		fprintf(stderr, "*Error occured during linking; status code: %d*\n", status);
		exit(status);
	}
}
