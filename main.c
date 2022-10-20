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

char cursorX, cursorY;

void cusor_show(char x, char y)
{
	spr_set(6, true, 24 + x * 32, 50 + 8 * 5 + y * 32, 16 + 108, VCOL_WHITE, false, true, true);
}

void cursor_move(signed char dx, signed char dy)
{
	dx += cursorX;
	dy += cursorY;
	if (dx >= 0 && dx < 9)
		cursorX = dx;
	if (dy >= 0 && dy < 5)
		cursorY = dy;

	unsigned	x = 24 + cursorX * 32;
	char		y = 50 + 8 * 5 + cursorY * 32;
	vic.spr_pos[6].x = x;
	vic.spr_pos[6].y = y;
	vic.spr_msbx = (vic.spr_msbx & 0xbf) | ((x & 0x100) != 0 ? 0x40 : 0x00);
	zombies_set_msbx(0x40, (x & 0x100) != 0 ? 0x40 : 0x00);
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

	static const unsigned prices[10] = {
		371, 25, 100, 250, 44, 30, 22, 111, 4987, 0
	};

	for(char i=0; i<10; i++)
	{
		menu_draw(i, i);
		menu_draw_price(i, prices[i]);
	}

	char	row = 0;
	for(;;)
	{
		char sirq = rirq_count;

		zombies_advance(row);
		plants_iterate(row);

		row++;
		if (row == 5)
			row = 0;

		shots_advance();

		if (!(rand() & 63))
			zombies_add(172, rand() % 5, rand() & 1);

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
			case KSCAN_SPACE | KSCAN_QUAL_DOWN:
				if (plant_grid[cursorY][cursorX].type == PT_NONE)
				{
					plant_place(cursorX, cursorY, PT_PEASHOOTER);
					plant_draw(cursorX, cursorY);
				}
				break;
		}

		while (sirq == rirq_count)
			;
	}


	return 0;
}
