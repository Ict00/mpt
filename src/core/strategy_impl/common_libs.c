#include "common_libs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../config/gconfig.h"

void link_into_lib() {
	char cmd[4096];
	sprintf(cmd, "ar rcs %s.a obj/*.o", CURRENT_TARGET.binary_name);
	
	int status = system(cmd);
	
	if (status != 0) {
		fprintf(stderr, "*Error occured during linking; status code: %d*\n", status);
		exit(status);
	}
}

void init_for_shared() {
	int new_length = strlen(CURRENT_TARGET.flags) + strlen(" -fPIC") + 1;
	if ((CURRENT_TARGET.flags = realloc(CURRENT_TARGET.flags, sizeof(char)*new_length))) {
		strcat(CURRENT_TARGET.flags, " -fPIC");
	}
}

void link_into_shared_lib() {
	char cmd[4096];
	sprintf(cmd, "%s -shared -o %s.so obj/*.o", CURRENT_TARGET.compiler, CURRENT_TARGET.binary_name);
	
	int status = system(cmd);
	
	if (status != 0) {
		fprintf(stderr, "*Error occured during linking; status code: %d*\n", status);
		exit(status);
	}

}
