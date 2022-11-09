#ifndef LEVELS_H
#define LEVELS_H

#include "zombies.h"
#include "gamemusic.h"

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

	LVC_END = 0xf0,
};

enum Seeds
{
	SF_SUNFLOWER	=	0x0001,
	SF_PEASHOOTER	=	0x0002,
	SF_WALLNUT		=	0x0004,
	SF_CHERRYBOMB	=	0x0008,

	SF_SUNSHROOM	=	0x0010,
	SF_CACTUS		=	0x0020,
	SF_POTATOMINE	=	0x0040,
	SF_REPEATER		=	0x0080,

	SF_SNOWPEA		=	0x0100,
	SF_CHOMPER		=	0x0200,
	SF_SHOVEL		=	0x8000
};

enum LevelFlags
{
	LF_DAY			=	0x0000,
	LF_NIGHT		=	0x0001,
	LF_CONVEYOR		=	0x0002
};

struct Level
{
	char					rows, flags;
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

