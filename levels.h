#ifndef LEVELS_H
#define LEVELS_H

#include "zombies.h"
#include "gamemusic.h"
#include "seeds.h"
#include "gamemenu.h"

// A level zombie sequence is defined by a list of commands
// each commands has a delay in the upper four bits and a
// type of zombie in the lower four bits
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

	LVC_END = 0x0f,
};

enum LevelFlags
{
	LF_DAY			=	0x0000,
	LF_NIGHT		=	0x0001,
	LF_CONVEYOR		=	0x0002,
	LF_SHOVEL		=	0x0004,
	LF_FIXSEED		=	0x0008,
	LF_GENERATED	=	0x0010
};

// Level specification
struct Level
{
	const char *			name;
	char					rows, flags, graves, slots;
	unsigned				sun;
	unsigned				seeds;	// Flags of available seeds
	const LevelCommand	*	cmds;	// List of zombie commands
	Tune					tune;
	PlantType				tutorialPlant;	// New plant to show at beginning
	const char *			tutorialText;	// Tutorial text for plant
};

extern const Level	*	GameLevels[];
extern const Level	*	level;

// Start the level with index li
GameResponse level_start(char li);

// Iterate the level on each frame
void level_iterate(void);

// Check if the level is complete
bool level_complete(void);

#pragma compile("levels.c")

#endif

