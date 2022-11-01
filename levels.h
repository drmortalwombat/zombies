#ifndef LEVELS_H
#define LEVELS_H

#include "zombies.h"

enum LevelCommand
{
	LVC_NOP,
	LVC_END,

	LVC_DELAY_10F,
	LVC_DELAY_20F,
	LVC_DELAY_1S,
	LVC_DELAY_2S,
	LVC_DELAY_5S,
	LVC_DELAY_10S,
	LVC_DELAY_15S,
	LVC_DELAY_20S,
	LVC_DELAY_30S,
	LVC_DELAY_60S,

	LVC_ZOMBIE,
	LVC_ZOMBIE_CONE,
	LVC_ZOMBIE_POLE,
	LVC_ZOMBIE_BUCKET,
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

struct Level
{
	char					rows;
	unsigned				sun;
	unsigned				seeds;
	const LevelCommand	*	cmds;
};

static const LevelCommand	TestLevelCmds1[] = {
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

static const LevelCommand	TestLevelCmds2[] = {
	LVC_DELAY_30S,
	LVC_ZOMBIE,
	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_DELAY_20S,
	LVC_ZOMBIE,
	
	LVC_DELAY_10S,
	LVC_ZOMBIE,
	LVC_DELAY_1S,
	LVC_ZOMBIE,

	LVC_DELAY_10S,
	LVC_DELAY_5S,
	LVC_ZOMBIE,
	LVC_DELAY_1S,
	LVC_ZOMBIE,

	LVC_DELAY_30S,
	LVC_ZOMBIE,

	LVC_DELAY_5S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_1S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_END
};

static const LevelCommand	TestLevelCmds3[] = {
	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_DELAY_20S,
	LVC_ZOMBIE,

	LVC_DELAY_20S,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_20S,
	LVC_ZOMBIE_CONE,

	LVC_DELAY_10S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_15S,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_DELAY_2S,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_END
};

static const LevelCommand	TestLevelCmds4[] = {
	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_DELAY_30S,
	LVC_ZOMBIE,
	LVC_DELAY_15S,
	LVC_ZOMBIE,

	LVC_DELAY_15S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_15S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_15S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_15S,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_15S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,

	LVC_DELAY_15S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,

	LVC_DELAY_30S,
	LVC_ZOMBIE,
	LVC_DELAY_1S,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,

	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_END
};

static const LevelCommand	TestLevelCmds6[] = {
	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_DELAY_20S,
	LVC_ZOMBIE,

	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_20S,
	LVC_ZOMBIE_CONE,

	LVC_DELAY_20S,
	LVC_ZOMBIE_POLE,

	LVC_DELAY_15S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,

	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,

	LVC_DELAY_10S,
	LVC_ZOMBIE_POLE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_30S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_POLE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_END
};

static const LevelCommand	TestLevelCmds7[] = {
	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_DELAY_30S,
	LVC_ZOMBIE,
	LVC_DELAY_20S,
	LVC_ZOMBIE,

	LVC_DELAY_15S,
	LVC_ZOMBIE,
	LVC_ZOMBIE,

	LVC_DELAY_20S,
	LVC_ZOMBIE_POLE,
	LVC_DELAY_20S,
	LVC_ZOMBIE_POLE,
	LVC_DELAY_20S,
	LVC_ZOMBIE_POLE,
	LVC_ZOMBIE_CONE,

	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_ZOMBIE,
	LVC_ZOMBIE,

	LVC_DELAY_30S,
	LVC_ZOMBIE_POLE,
	LVC_ZOMBIE,

	LVC_DELAY_30S,
	LVC_ZOMBIE,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_1S,
	LVC_ZOMBIE,
	LVC_ZOMBIE,
	LVC_ZOMBIE,
	LVC_DELAY_1S,
	LVC_ZOMBIE,
	LVC_ZOMBIE,
	LVC_DELAY_1S,
	LVC_ZOMBIE,
	LVC_ZOMBIE,

	LVC_DELAY_15S,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,

	LVC_DELAY_15S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_10S,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,

	LVC_DELAY_15S,
	LVC_ZOMBIE,
	LVC_ZOMBIE,
	LVC_DELAY_1S,
	LVC_ZOMBIE,
	LVC_ZOMBIE,
	LVC_ZOMBIE,

	LVC_DELAY_10S,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,

	LVC_DELAY_10S,
	LVC_ZOMBIE_POLE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_POLE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_10S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_POLE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_10S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_POLE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_10S,
	LVC_ZOMBIE_POLE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_POLE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_POLE,

	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_POLE,
	LVC_DELAY_10F,	
	LVC_ZOMBIE,
	LVC_DELAY_10F,	
	LVC_ZOMBIE,
	LVC_DELAY_10F,	
	LVC_ZOMBIE,
	LVC_DELAY_10F,	
	LVC_ZOMBIE,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_DELAY_10F,	
	LVC_ZOMBIE,
	LVC_DELAY_10F,	

	LVC_END
};

static const LevelCommand	TestLevelCmds8[] = {
	LVC_DELAY_30S,
	LVC_ZOMBIE,
	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_DELAY_30S,
	LVC_ZOMBIE,

	LVC_DELAY_20S,
	LVC_ZOMBIE_CONE,

	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_ZOMBIE,

	LVC_DELAY_15S,
	LVC_ZOMBIE_BUCKET,

	LVC_DELAY_20S,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_15S,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_15S,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_30S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_BUCKET,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_END
};


static const LevelCommand	TestLevelCmds9[] = {
	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_DELAY_30S,
	LVC_ZOMBIE,
	LVC_DELAY_30S,
	LVC_ZOMBIE,

	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_15S,
	LVC_ZOMBIE_POLE,

	LVC_DELAY_20S,
	LVC_ZOMBIE_CONE,
	
	LVC_DELAY_15S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,

	LVC_DELAY_15S,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_15S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_POLE,

	LVC_DELAY_20S,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_BUCKET,
	LVC_ZOMBIE,

	LVC_DELAY_15S,
	LVC_ZOMBIE_CONE,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_10S,
	LVC_ZOMBIE_CONE,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_10S,
	LVC_ZOMBIE_BUCKET,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_10S,
	LVC_ZOMBIE_CONE,
	LVC_ZOMBIE_POLE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_10S,
	LVC_ZOMBIE_CONE,
	LVC_ZOMBIE_POLE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_10S,
	LVC_ZOMBIE_BUCKET,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,

	LVC_DELAY_10S,
	LVC_ZOMBIE_POLE,
	LVC_ZOMBIE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,
	LVC_ZOMBIE,

	LVC_DELAY_10S,
	LVC_ZOMBIE_BUCKET,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,


	LVC_DELAY_10S,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_CONE,
	LVC_DELAY_10F,
	LVC_ZOMBIE_POLE,
	LVC_DELAY_10F,
	LVC_ZOMBIE,

	LVC_DELAY_30S,
	LVC_ZOMBIE,
	LVC_DELAY_1S,

	LVC_ZOMBIE,
	LVC_ZOMBIE,
	LVC_ZOMBIE,
	LVC_DELAY_1S,

	LVC_ZOMBIE,
	LVC_ZOMBIE_CONE,
	LVC_ZOMBIE,
	LVC_DELAY_1S,

	LVC_ZOMBIE,
	LVC_ZOMBIE_BUCKET,
	LVC_ZOMBIE,
	LVC_DELAY_1S,

	LVC_ZOMBIE,
	LVC_ZOMBIE_POLE,
	LVC_ZOMBIE,

	LVC_END
};

static const Level	TestLevel1 = {
	0b00100,
	300,
	SF_PEASHOOTER,
	TestLevelCmds1
};

static const Level	TestLevel2 = {
	0b01110,
	50,
	SF_SUNFLOWER | SF_PEASHOOTER,
	TestLevelCmds2
};

static const Level	TestLevel3 = {
	0b01110,
	50,
	SF_SUNFLOWER | SF_PEASHOOTER | SF_CHERRYBOMB,
	TestLevelCmds3
};

static const Level	TestLevel4 = {
	0b11111,
	50,
	SF_SUNFLOWER | SF_PEASHOOTER | SF_CHERRYBOMB | SF_WALLNUT,
	TestLevelCmds4
};

static const Level	TestLevel6 = {
	0b11111,
	50,
	SF_SUNFLOWER | SF_PEASHOOTER | SF_CHERRYBOMB | SF_WALLNUT | SF_POTATOMINE | SF_SHOVEL,
	TestLevelCmds6
};

static const Level	TestLevel7 = {
	0b11111,
	50,
	SF_SUNFLOWER | SF_PEASHOOTER | SF_CHERRYBOMB | SF_WALLNUT | SF_POTATOMINE | SF_SNOWPEA | SF_SHOVEL,
	TestLevelCmds7
};

static const Level	TestLevel8 = {
	0b11111,
	50,
	SF_SUNFLOWER | SF_PEASHOOTER | SF_CHERRYBOMB | SF_WALLNUT | SF_POTATOMINE | SF_SNOWPEA | SF_CHOMPER | SF_SHOVEL,
	TestLevelCmds8
};

static const Level	TestLevel9 = {
	0b11111,
	50,
	SF_SUNFLOWER | SF_PEASHOOTER | SF_CHERRYBOMB | SF_WALLNUT | SF_POTATOMINE | SF_SNOWPEA | SF_CHOMPER | SF_REPEATER | SF_SHOVEL,
	TestLevelCmds9
};

void level_start(const Level * l);

void level_iterate(void);

bool level_complete(void);

#pragma compile("levels.c")

#endif
