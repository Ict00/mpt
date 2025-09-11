#include "common.h"

#include <linux/limits.h>
#include <stdlib.h>
#include <unistd.h>
#include "../../config/gconfig.h"
#include "../../utils.h"

void common_done() {
	system(CURRENT_TARGET.post_cmd);
}

void common_start() {
	system(CURRENT_TARGET.pre_cmd);
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
