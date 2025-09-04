#include "strategy.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "strategy_impl/c_console.h"
#include "../utils.h"

strategy_t c_console;
strategy_t dummy;

void init_strategies() {

	static step_t cc_steps[3] = {
		{cct_compile, "Building C files", 0},
		{cct_link, "Linking...", 0},
		{cct_done, "Executing Post CMD (if there is any)", 0}
	};

	static step_t dm_steps[1] = {
		{cct_done, "Executing CMD", 0}
	};

	c_console = (strategy_t){.name="C: Console", .steps=cc_steps, .steps_count=3};
	dummy = (strategy_t){.name="Dummy", .steps=dm_steps, .steps_count=1};
	
}

void do_strategy(strategy_t strategy) {
	printf("\x1b[1m" PROJ_NAME "\x1b[0m\nBuilding with '%s'\n\n\n\n", strategy.name);
	for (int i = 0; i < strategy.steps_count; ++i) {
		char* _bar = bar(30, strategy.steps_count, i);
		out_status(strategy.steps[i].description, _bar, strategy.steps_count, i);
		strategy.steps[i].exec();
		usleep(strategy.steps[i].timeout*10000);	
	}
	char* _bar = bar(30, 1, 1);
	printf("\x1b[K\x1b[2A\x1b[200D%s\n\x1b[KDone!\n", _bar);
	free(_bar);
	
	fflush(stdout);
}
