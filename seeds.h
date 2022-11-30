#ifndef SEEDS_H
#define SEEDS_H

#include "plants.h"

enum Seeds
{
	SD_PEASHOOTER,
	SD_SUNFLOWER,
	SD_CHERRYBOMB,
	SD_WALLNUT,

	SD_POTATOMINE,
	SD_SNOWPEA,
	SD_CHOMPER,
	SD_REPEATER,

	SD_PUFFSHROOM,
	SD_SUNSHROOM,
	SD_FUMESHROOM,
	SD_GRAVEDIGGER,

	SD_SCAREDYSHROOM,
	SD_ICESHROOM,
	SD_DOOMSHROOM
};

enum SeedFlags
{
	SF_PEASHOOTER		=	1u << SD_PEASHOOTER,
	SF_SUNFLOWER		=	1u << SD_SUNFLOWER,
	SF_CHERRYBOMB		=	1u << SD_CHERRYBOMB,
	SF_WALLNUT			=	1u << SD_WALLNUT,

	SF_POTATOMINE		=	1u << SD_POTATOMINE,
	SF_SNOWPEA			=	1u << SD_SNOWPEA,
	SF_CHOMPER			=	1u << SD_CHOMPER,
	SF_REPEATER			=	1u << SD_REPEATER,

	SF_PUFFSHROOM		=	1u << SD_PUFFSHROOM,
	SF_SUNSHROOM		=	1u << SD_SUNSHROOM,
	SF_FUMESHROOM		=	1u << SD_FUMESHROOM,
	SF_GRAVEDIGGER		=	1u << SD_GRAVEDIGGER,
	
	SF_SCAREDYSHROOM	=	1u << SD_SCAREDYSHROOM,
	SF_ICESHROOM		=	1u << SD_ICESHROOM,
	SF_DOOMSHROOM		=	1u << SD_DOOMSHROOM
};

struct SeedInfo
{
	PlantType	plant;
	unsigned	cost;
	char		warm;
	bool		ready;
};

extern const SeedInfo seed_info[16];

void seeds_edit_menu(SeedFlags seeds, char slots);

#pragma compile("seeds.c")

#endif
