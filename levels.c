#include "levels.h"

const Level	*	level;
char			level_cmd;
unsigned		level_delay;

void level_start(const Level * l)
{
	level = l;
	level_cmd = 0;
	level_delay = 0;
}

void level_iterate(void)
{
	if (level_delay)
		level_delay--;
	else
	{
		switch (level->cmds[level_cmd])
		{
			case LVC_NOP:
				level_cmd++;
				break;

			case LVC_DELAY_5F:
				level_delay = 1;
				level_cmd++;
				break;
			case LVC_DELAY_10F:
				level_delay = 2;
				level_cmd++;
				break;
			case LVC_DELAY_20F:
				level_delay = 4;
				level_cmd++;
				break;
			case LVC_DELAY_1S:
				level_delay = 10;
				level_cmd++;
				break;
			case LVC_DELAY_5S:
				level_delay = 50;
				level_cmd++;
				break;
			case LVC_DELAY_10S:
				level_delay = 100;
				level_cmd++;
				break;
			case LVC_DELAY_20S:
				level_delay = 200;
				level_cmd++;
				break;
			case LVC_DELAY_60S:
				level_delay = 600;
				level_cmd++;
				break;

			case LVC_ZOMBIE:
				zombies_add(172, rand() % 5, ZOMBIE_BASE);
				level_cmd++;				
				break;

			case LVC_ZOMBIE_CONE:
				zombies_add(172, rand() % 5, ZOMBIE_CONE);
				level_cmd++;				
				break;			
		}
	}
}

bool level_complete(void)
{
	return level->cmds[level_cmd] == LVC_END;
}

