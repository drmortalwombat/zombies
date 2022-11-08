#include <c64/vic.h>
#include <c64/sprites.h>
#include <c64/rasterirq.h>
#include <c64/memmap.h>
#include <c64/keyboard.h>
#include <c64/joystick.h>
#include <oscar.h>
#include <string.h>
#include <stdlib.h>
#include "gamemusic.h"
#include "display.h"
#include "plants.h"
#include "zombies.h"
#include "levels.h"

#pragma region( main, 0x0a00, 0x9800, , , {code, data, bss, heap, stack} )
#pragma stacksize(1024)

signed char cursorX, cursorY, menuX;
unsigned	sun_count;

void cusor_show(char x, char y)
{
	spr_set(6, true, 24 + x * 32, 50 + 8 * 5 + y * 32, 16 + 108, VCOL_WHITE, false, true, true);
}

void cursor_move(signed char dx, signed char dy)
{
	cursorX += dx;
	cursorY += dy;
	if (cursorY < 0)
	{
		cursorY = -1;
		if (cursorX < menu_first)
			cursorX = 1;
		else if (cursorX >= menu_size)
			cursorX = menu_size-1;
		menuX = cursorX;
	}
	else
	{
		if (cursorY > 4)
			cursorY = 4;
		if (cursorX < 0)
			cursorX = 0;
		else if (cursorX > 8)
			cursorX = 8;
	}

	
	if (cursorY < 0)
	{
		unsigned	x = 24 + cursorX * 32;
		rirq_data(&menuMux, 1, x);
		rirq_data(&menuMux, 2, (x & 0x100) != 0 ? 0x40 : 0x00);
		rirq_data(&cursorMux, 3, 16 + 110);
	}
	else
	{
		char		y = 50 + 8 * 5 + cursorY * 32;
		unsigned	x = 40 + cursorX * 32;

		rirq_data(&cursorMux, 0, y);
		rirq_data(&cursorMux, 1, x);
		rirq_data(&cursorMux, 2, (x & 0x100) != 0 ? 0x40 : 0x00);
		rirq_data(&cursorMux, 3, 16 + 108);
		zombies_set_msbx(0x40, (x & 0x100) != 0 ? 0x40 : 0x00);
	}
}

void menu_set(char m)
{
	if (m >= menu_first && m < menu_size)
	{
		menuX = m;
		unsigned	x = 24 + m * 32;
		rirq_data(&menuMux, 1, x);
		rirq_data(&menuMux, 2, (x & 0x100) != 0 ? 0x40 : 0x00);
	}
}

void cursor_select(void)
{
	if (cursorY >= 0)
	{
		if (menu[menuX].type == PT_SHOVEL)
		{
			if (menu[menuX].cool == 0 && plant_grid[cursorY][cursorX].type > PT_GROUND)
			{
				menu_cooldown(menuX);
				plant_remove(cursorX, cursorY);
				plant_draw(cursorX, cursorY);						
			}
		}
		else if (menu[menuX].type != PT_CONVEYOR)
		{
			if (plant_grid[cursorY][cursorX].type < PT_GROUND)
			{
				if (menu[menuX].cool == 0 && menu[menuX].price <= menu[0].price)
				{
					menu[0].price -= menu[menuX].price;
					menu_draw_price(0, menu[0].price);
					plant_place(cursorX, cursorY, menu[menuX].type);
					plant_draw(cursorX, cursorY);
					menu_cooldown(menuX);
				}
			}
		}
	}
}

void game_level_loop(void)
{
	char	row = 0, warm = 0;
	sun_count	= 500;

	char	joydown = 0;
	char	step = 1;
	for(;;)
	{
		char sirq = rirq_count;

		for(char i=0; i<step; i++)
		{
			if (row & 1)
				zombies_advance(row >> 1);
			else
				plants_iterate(row >> 1);

			row++;
			if (row == 10)
			{
				row = 0;
				level_iterate();
				if (level_complete() && zombies_done())
					return;
			}

		}

		warm += step;
		if (warm >= 25)
		{
			warm -= 25;
			menu_warmup();
		}

		shots_advance(step);

		sun_advance();

		if (!(level->flags & LF_CONVEYOR))
		{
			if (sun_count >= step)
				sun_count -= step;
			else if (!sun_active)
			{
				sun_add(8, 50, 200, 25);
				sun_count = 500;
			}
		}

		keyb_poll();
		switch (keyb_key)
		{
			case KSCAN_CSR_RIGHT | KSCAN_QUAL_DOWN:
				cursor_move(1, 0);
				break;
			case KSCAN_CSR_RIGHT | KSCAN_QUAL_SHIFT | KSCAN_QUAL_DOWN:
				cursor_move(-1, 0);
				break;
			case KSCAN_CSR_DOWN | KSCAN_QUAL_DOWN:
				cursor_move(0, 1);
				break;
			case KSCAN_CSR_DOWN | KSCAN_QUAL_SHIFT | KSCAN_QUAL_DOWN:
				cursor_move(0, -1);
				break;
			case KSCAN_0 | KSCAN_QUAL_DOWN:
				menu_set(0);
				break;
			case KSCAN_1 | KSCAN_QUAL_DOWN:
				menu_set(1);
				break;
			case KSCAN_2 | KSCAN_QUAL_DOWN:
				menu_set(2);
				break;
			case KSCAN_3 | KSCAN_QUAL_DOWN:
				menu_set(3);
				break;
			case KSCAN_4 | KSCAN_QUAL_DOWN:
				menu_set(4);
				break;
			case KSCAN_5 | KSCAN_QUAL_DOWN:
				menu_set(5);
				break;
			case KSCAN_6 | KSCAN_QUAL_DOWN:
				menu_set(6);
				break;
			case KSCAN_7 | KSCAN_QUAL_DOWN:
				menu_set(7);
				break;
			case KSCAN_8 | KSCAN_QUAL_DOWN:
				menu_set(8);
				break;
			case KSCAN_9 | KSCAN_QUAL_DOWN:
				menu_set(9);
				break;
			case KSCAN_SPACE | KSCAN_QUAL_DOWN:
				cursor_select();
				break;
		}

		joy_poll(0);
		if (joydown)
		{
			joydown--;
			if (!joyx[0] && !joyy[0] && !joyb[0])
				joydown = 0;
		}
		else
		{
			if (joyx[0] || joyy[0])
			{
				cursor_move(joyx[0], joyy[0]);
				joydown = 20;
			}
			if (joyb[0])
			{
				cursor_select();			
				joydown = 20;
			}
		}

		if (sirq == rirq_count)
		{
			plants_animate();
		}

		while (sirq == rirq_count)
			;

		step = rirq_count - sirq;
	}
}

int main(void)
{
	display_init();

	for(char level=0; level<9; level++)
	{
		shots_init();
		zombies_init();
		plant_draw_borders();

		level_start(GameLevels[level]);

		menu_set(1);
		cusor_show(0, 0);
		cursor_move(0, 0);

		for(char y=0; y<5; y++)
			for(char x=0; x<9; x++)
				plant_draw(x, y);

		game_level_loop();


		music_init(TUNE_VICTORY);

		for(int i=0; i<190; i++)
			vic_waitFrame();
	}


	return 0;
}
