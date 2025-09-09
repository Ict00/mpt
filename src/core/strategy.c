#include "strategy.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "strategy_impl/c_console.h"
#include "strategy_impl/common.h"
#include "strategy_impl/common_libs.h"
#include "strategy_impl/cpp_console.h"
#include "../utils.h"

strategy_t cpp_static_lib;
strategy_t cpp_shared_lib;
strategy_t c_static_lib;
strategy_t c_shared_lib;
strategy_t c_console;
strategy_t cpp_console;
strategy_t dummy;

void init_strategies() {

	static step_t cc_steps[4] = {
		{common_start, "Executing Init CMD (if there is any)", 0},
		{cct_compile, "Building C files", 0},
		{cct_link, "Linking...", 0},
		{common_done, "Executing Post CMD (if there is any)", 0}
	};

	static step_t cppc_steps[4] = {
		{common_start, "Executing Init CMD (if there is any)", 0},
		{cpp_c_compile, "Building C++ files", 0},
		{cct_link, "Linking...", 0},
		{common_done, "Executing Post CMD (if there is any)"}
	};
	
	static step_t cstl_steps[4] = {
		{common_start, "Executing Init CMD (if there is any)", 0},
		{cct_compile, "Building C files", 0},
		{link_into_lib, "Linking...", 0},
		{common_done, "Executing Post CMD (if there is any)"}
	};
	
	static step_t cppstl_steps[4] = {
		{common_start, "Executing Init CMD (if there is any)", 0},
		{cpp_c_compile, "Building C++ files", 0},
		{link_into_lib, "Linking...", 0},
		{common_done, "Executing Post CMD (if there is any)"}
	};

	static step_t cppshl_steps[5] = {
		{common_start, "Executing Init CMD (if there is any)", 0},
		{init_for_shared, "Editing 'flags' field", 0},
		{cpp_c_compile, "Building C++ files", 0},
		{link_into_shared_lib, "Linking...", 0},
		{common_done, "Executing Post CMD (if there is any)"}
	};

	static step_t cshl_steps[5] = {
		{common_start, "Executing Init CMD (if there is any)", 0},
		{init_for_shared, "Editing 'flags' field", 0},
		{cct_compile, "Building C files", 0},
		{link_into_shared_lib, "Linking...", 0},
		{common_done, "Executing Post CMD (if there is any)"}
	};

	static step_t dm_steps[1] = {
		{common_done, "Executing CMD", 0}
	};

	c_console = (strategy_t){.name="C: Console", .steps=cc_steps, .steps_count=4};
	c_static_lib = (strategy_t){.name="C: Static Lib", .steps=cstl_steps, .steps_count=4};
	c_shared_lib = (strategy_t){.name="C: Shared Lib", .steps=cshl_steps, .steps_count=5};
	cpp_static_lib = (strategy_t){.name="C++: Static Lib", .steps=cppstl_steps, .steps_count=4};
	cpp_shared_lib = (strategy_t){.name="C++: Shared Lib", .steps=cppshl_steps, .steps_count=5};
	cpp_console = (strategy_t){.name="C++: Console", .steps=cppc_steps, .steps_count=4};
	dummy = (strategy_t){.name="Dummy", .steps=dm_steps, .steps_count=1};

}

void do_strategy(strategy_t strategy) {
	printf("\x1b[1mMPT\x1b[0m\nBuilding with '%s'\n\n\n\n", strategy.name);
	for (int i = 0; i < strategy.steps_count; ++i) {
		char* _bar = bar(30, strategy.steps_count, i);
		out_status(strategy.steps[i].description, _bar, strategy.steps_count, i+1);
		strategy.steps[i].exec();
		usleep(strategy.steps[i].timeout*10000);	
	}
	char* _bar = bar(30, 1, 1);
	printf("\x1b[K\x1b[2A\x1b[200D%s\n\x1b[KDone!\n", _bar);
	free(_bar);
	
	fflush(stdout);
	exit(0);
}
