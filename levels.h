#ifndef LEVELS_H
#define LEVELS_H

#include "zombies.h"
#include "gamemusic.h"
#include "seeds.h"

enum LevelCommand
{
	LVC_DELAY_10F = 0x00,
	LVC_DELAY_20F = 0x10,
	LVC_DELAY_30F = 0x20,

	LVC_DELAY_1S = 0x30,
	LVC_DELAY_2S = 0x40,
	LVC_DELAY_5S = 0x50,
	LVC_DELAY_10S = 0x60,
	LVC_DELAY_15S = 0x70,
	LVC_DELAY_20S = 0x80,
	LVC_DELAY_25S = 0x90,
	LVC_DELAY_30S = 0xa0,

	LVC_ZOMBIE = 0x00,
	LVC_ZOMBIE_CONE = 0x01,
	LVC_ZOMBIE_POLE = 0x02,
	LVC_ZOMBIE_BUCKET = 0x03,
	LVC_ZOMBIE_PAPER = 0x04,
	LVC_ZOMBIE_SCREENDOOR = 0x05,
	LVC_ZOMBIE_FOOTBALL = 0x06,
	LVC_ZOMBIE_DANCER = 0x07,

	LVC_ZOMBIE_GRAVES = 0x08,

	LVC_END = 0xf0,
};

enum LevelFlags
{
	LF_DAY			=	0x0000,
	LF_NIGHT		=	0x0001,
	LF_CONVEYOR		=	0x0002,
	LF_SHOVEL		=	0x0004,
	LF_FIXSEED		=	0x0008
};

struct Level
{
	const char *			name;
	char					rows, flags, graves, slots;
	unsigned				sun;
	unsigned				seeds;
	const LevelCommand	*	cmds;
	Tune					tune;
};

extern const Level	*	GameLevels[];
extern const Level	*	level;

void level_start(const Level * l);

void level_iterate(void);

bool level_complete(void);

#pragma compile("levels.c")

#endif

