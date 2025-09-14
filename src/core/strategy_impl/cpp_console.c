#include "cpp_console.h"

#include "../../config/gconfig.h"
#include "../../utils.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int compiled2 = 0;

void ccp_c_compile_sf(char *l) {
	if (!ends_with(l, ".cpp"))
		return;

	char cmd[1024];
	snprintf(cmd, sizeof(cmd), "%s %s -o obj/%d.o %s", CURRENT_TARGET.compiler, l, compiled2, CURRENT_TARGET.flags);

	int status = system(cmd);

	if (status != 0) {
		fprintf(stderr, "*Error occured during compilation; status code: %d*\n", status);
		exit(status);
	}

	compiled2++;
}

void cpp_c_compile() {
	char buffer[PATH_MAX];
	int bi = 0;
	int i = 0;
	for (; CURRENT_TARGET.sources[i] != 0; i++) {
		if (CURRENT_TARGET.sources[i] == ':') {
			buffer[bi] = 0;
			list_dir(buffer, ccp_c_compile_sf, true, false);
			bi = 0;
			continue;
		}

		buffer[bi] = CURRENT_TARGET.sources[i];
		bi++;
	}
	if (CURRENT_TARGET.sources[i] == 0) {
		buffer[bi] = 0;
		list_dir(buffer, ccp_c_compile_sf, true, false);
	}
}
