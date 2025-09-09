#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include "../../config/gconfig.h"

void common_done() {
	system(CURRENT_TARGET.post_cmd);
}

void common_start() {
	system(CURRENT_TARGET.pre_cmd);
}
