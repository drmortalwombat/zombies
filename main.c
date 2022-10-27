#include <c64/vic.h>
#include <c64/sprites.h>
#include <c64/rasterirq.h>
#include <c64/memmap.h>
#include <c64/keyboard.h>
#include <oscar.h>
#include <string.h>
#include <stdlib.h>
#include "gamemusic.h"
#include "display.h"
#include "plants.h"
#include "zombies.h"
#include "levels.h"

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
		if (cursorX < 1)
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

	unsigned	x = 24 + cursorX * 32;
	
	if (cursorY < 0)
	{
		rirq_data(&menuMux, 1, x);
		rirq_data(&menuMux, 2, (x & 0x100) != 0 ? 0x40 : 0x00);
		rirq_data(&cursorMux, 3, 16 + 110);
	}
	else
	{
		char		y = 50 + 8 * 5 + cursorY * 32;

		rirq_data(&cursorMux, 0, y);
		rirq_data(&cursorMux, 1, x);
		rirq_data(&cursorMux, 2, (x & 0x100) != 0 ? 0x40 : 0x00);
		rirq_data(&cursorMux, 3, 16 + 108);
		zombies_set_msbx(0x40, (x & 0x100) != 0 ? 0x40 : 0x00);
	}
}

void menu_set(char m)
{
	if (m >= 1 && m < menu_size)
	{
		menuX = m;
		unsigned	x = 24 + m * 32;
		rirq_data(&menuMux, 1, x);
		rirq_data(&menuMux, 2, (x & 0x100) != 0 ? 0x40 : 0x00);
	}
}

int main(void)
{
	display_init();

	plant_grid_clear();
	shots_init();
	zombies_init();

	for(char y=0; y<5; y++)
	{
		for(char x=0; x<10; x++)
		{
//			char r = rand() & 31;
//			if (x < 6 && r < 8)
//				plant_place(x, y, PT_PEASHOOTER);
			plant_draw(x, y);
		}
	}

#if 0
	for(int i=0; i<5; i++)
	{
		for(int j=0; j<3; j++)
		{
			zombies_add(j * 32 + 300, i);
		}
	}
#endif

	cusor_show(0, 0);


	menu_init();
	menu_add_item(PT_SUN, 400, 0);
	menu_add_item(PT_SUNFLOWER, 50, 17);
	menu_add_item(PT_PEASHOOTER, 100, 17);
	menu_add_item(PT_SNOWPEA, 175, 17);
	menu_add_item(PT_WALLNUT, 50, 6);
	menu_add_item(PT_POTATOMINE, 25, 6);
	menu_add_item(PT_REPEATER, 200, 17);
	menu_add_item(PT_CHERRYBOMB, 150, 3);
	menu_add_item(PT_CHOMPER, 150, 17);
	menu_add_item(PT_SHOVEL, 0, 17);
	menu_set(1);
	cursor_move(0, 0);

	level_start(&TestLevel);

	char	row = 0, warm = 0;
	sun_count	= 500;
	for(;;)
	{
		char sirq = rirq_count;

		zombies_advance(row);
		plants_iterate(row);

		row++;
		if (row == 5)
		{
			row = 0;
			level_iterate();
		}

		warm++;
		if (warm == 25)
		{
			warm = 0;
			menu_warmup();
		}

		shots_advance();
		sun_advance();

		if (sun_count > 0)
			sun_count--;
		else if (!sun_active)
		{
			sun_add(8, 50, 200, 50);
			sun_count = 500;
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
				if (cursorY >= 0)
				{
					if (menu[menuX].type == PT_SHOVEL)
					{
						if (menu[menuX].cool == 0 && plant_grid[cursorY][cursorX].type != PT_NONE)
						{
							menu_cooldown(menuX);
							plant_remove(cursorX, cursorY);
							plant_draw(cursorX, cursorY);						
						}
					}
					else
					{
						if (plant_grid[cursorY][cursorX].type == PT_NONE)
						{
							if (menu[menuX].cool == 0 && menu[menuX].price <= menu[0].price)
							{
								menu[0].price -= menu[menuX].price;
								menu_cooldown(menuX);
								menu_draw_price(0, menu[0].price);
								plant_place(cursorX, cursorY, menu[menuX].type);
								plant_draw(cursorX, cursorY);
							}
						}
					}
				}
				break;
		}

		while (sirq == rirq_count)
			;
	}


	return 0;
}
