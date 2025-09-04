#pragma once

#include <stddef.h>

typedef void (*st_exec)(void);

struct {
	st_exec exec;
	const char* description;
	int timeout;
} typedef step_t;

struct {
	step_t* steps;
	int steps_count;
	const char* name;
} typedef strategy_t;

extern strategy_t c_console;
extern strategy_t dummy;

void init_strategies();
void do_strategy(strategy_t strategy);
