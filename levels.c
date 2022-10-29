#include "levels.h"
#include "plants.h"

const Level	*	level;
char			level_cmd;
unsigned		level_delay;

char			level_rows[32];


void level_start(const Level * l)
{
	level = l;
	level_cmd = 0;
	level_delay = 0;

	char row = 0;
	for(char i=0; i<32; i++)
	{		
		do
		{
			row++;
			if (row == 5)
				row = 0;
		}
		while (!(l->rows & (1 << row)));

		level_rows[i] = row;
	}

	menu_init();
	menu_add_item(PT_SUN, l->sun, 0, true);

	unsigned	seeds = l->seeds;
	if (seeds & SF_SUNFLOWER)
		menu_add_item(PT_SUNFLOWER, 50, 17, true);
	if (seeds & SF_PEASHOOTER)
		menu_add_item(PT_PEASHOOTER, 100, 17, true);
	if (seeds & SF_SNOWPEA)
		menu_add_item(PT_SNOWPEA, 175, 17, false);
	if (seeds & SF_WALLNUT)
		menu_add_item(PT_WALLNUT, 50, 6, false);
	if (seeds & SF_POTATOMINE)
		menu_add_item(PT_POTATOMINE, 25, 6, false);
	if (seeds & SF_REPEATER)
		menu_add_item(PT_REPEATER, 200, 17, false);
	if (seeds & SF_CHERRYBOMB)
		menu_add_item(PT_CHERRYBOMB, 150, 3, false);
	if (seeds & SF_CHOMPER)
		menu_add_item(PT_CHOMPER, 150, 17, false);
	if (seeds & SF_SHOVEL)
		menu_add_item(PT_SHOVEL, 0, 17, false);

	plant_grid_clear(l->rows);
}

void level_iterate(void)
{
	if (level_delay)
		level_delay--;
	else
	{
		char 	i = rand() & 15;
		char	row = level_rows[i];
		while (i < 31)
		{
			level_rows[i] = level_rows[i + 1];
			i++;
		}
		level_rows[31] = row;

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
			case LVC_DELAY_2S:
				level_delay = 20;
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
			case LVC_DELAY_15S:
				level_delay = 150;
				level_cmd++;
				break;
			case LVC_DELAY_20S:
				level_delay = 200;
				level_cmd++;
				break;
			case LVC_DELAY_30S:
				level_delay = 300;
				level_cmd++;
				break;
			case LVC_DELAY_60S:
				level_delay = 600;
				level_cmd++;
				break;

			case LVC_ZOMBIE:
				zombies_add(172, row, ZOMBIE_BASE);
				level_cmd++;				
				break;

			case LVC_ZOMBIE_CONE:
				zombies_add(172, row, ZOMBIE_CONE);
				level_cmd++;				
				break;			

			case LVC_ZOMBIE_POLE:
				zombies_add(172, row, ZOMBIE_POLE);
				level_cmd++;				
				break;			
		}
	}
}

bool level_complete(void)
{
	return level->cmds[level_cmd] == LVC_END;
}

