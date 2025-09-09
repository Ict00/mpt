#include "common.h"

#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "../../config/gconfig.h"
#include "../../utils.h"

void common_done() {
	system(CURRENT_TARGET.post_cmd);
}

void common_start() {
	system(CURRENT_TARGET.pre_cmd);
}
