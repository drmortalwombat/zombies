#ifndef LEVELS_H
#define LEVELS_H

#include "zombies.h"

enum LevelCommand
{
	LVC_NOP,
	LVC_END,

	LVC_DELAY_5F,
	LVC_DELAY_10F,
	LVC_DELAY_20F,
	LVC_DELAY_1S,
	LVC_DELAY_5S,
	LVC_DELAY_10S,
	LVC_DELAY_20S,
	LVC_DELAY_60S,

	LVC_ZOMBIE,
	LVC_ZOMBIE_CONE,
};

struct Level
{
	const LevelCommand	*	cmds;
};

static const LevelCommand	TestLevelCmds[] = {
	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_DELAY_5S,
	LVC_ZOMBIE,
	LVC_DELAY_5S,
	LVC_ZOMBIE,
	LVC_DELAY_1S,
	LVC_ZOMBIE,
	LVC_DELAY_5S,
	LVC_ZOMBIE,
	LVC_DELAY_20F,
	LVC_ZOMBIE,
	LVC_END
};

static const LevelCommand	TestLevelCmds3[] = {
	LVC_DELAY_5S,
	LVC_ZOMBIE,
	LVC_DELAY_5F,
	LVC_ZOMBIE,
	LVC_DELAY_5F,
	LVC_ZOMBIE,
	LVC_DELAY_5F,
	LVC_ZOMBIE,
	LVC_DELAY_5F,
	LVC_ZOMBIE,
	LVC_DELAY_5F,
	LVC_ZOMBIE,
	LVC_DELAY_5F,
	LVC_ZOMBIE,
	LVC_DELAY_5F,
	LVC_ZOMBIE,
	LVC_DELAY_5F,
	LVC_ZOMBIE,
	LVC_DELAY_5F,
	LVC_ZOMBIE,
	LVC_DELAY_5F,
	LVC_ZOMBIE,
	LVC_DELAY_5F,
	LVC_END	
};

static const Level	TestLevel = {
	TestLevelCmds3
};

void level_start(const Level * l);

void level_iterate(void);

bool level_complete(void);

#pragma compile("levels.c")

#endif

