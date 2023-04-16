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
#include "lawnmower.h"
#include "seeds.h"
#include "gamemenu.h"

#pragma region( stack, 0x0400, 0x0800, , , {stack})

#pragma region( main, 0x0880, 0x9400, , , {code, data, bss, heap} )
#pragma stacksize(1024)

signed char cursorX, cursorY, menuX;
unsigned	sun_count;

void cursor_show(char x, char y)
{
	cursorX = x;
	cursorY = y;
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

void menu_move(signed char dx)
{
	if (dx < 0 && menuX > menu_first)
		menuX--;
	else if (dx > 0 && menuX + 1 < menu_size)
		menuX++;
	else
		return;

	unsigned	x = 24 + menuX * 32;
	rirq_data(&menuMux, 1, x);
	rirq_data(&menuMux, 2, (x & 0x100) != 0 ? 0x40 : 0x00);
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
		switch (menu[menuX].type)
		{
			case PT_SHOVEL:
				if (menu[menuX].cool == 0 && plant_grid[cursorY][cursorX].type > PT_GROUND)
				{
					plant_remove(cursorX, cursorY);
					plant_draw(cursorX, cursorY);						
				}
				break;

			case PT_GRAVEDIGGER:

				if (menu[menuX].cool == 0 && menu[menuX].price <= menu[0].price && plant_grid[cursorY][cursorX].type == PT_TOMBSTONE)
				{
					menu[0].price -= menu[menuX].price;
					disp_put_price(menu[0].price, 0, 4);
					plant_place(cursorX, cursorY, menu[menuX].type);
					plant_draw(cursorX, cursorY);
					menu_cooldown(menuX);
				}
				break;

			case PT_CONVEYOR:
				break;

			default:
				if (plant_grid[cursorY][cursorX].type < PT_TOMBSTONE)
				{
					if (menu[menuX].cool == 0 && menu[menuX].price <= menu[0].price)
					{
						menu[0].price -= menu[menuX].price;
						disp_put_price(menu[0].price, 0, 4);
						plant_place(cursorX, cursorY, menu[menuX].type);
						plant_draw(cursorX, cursorY);
						menu_cooldown(menuX);
					}
				}
				break;
		}
	}
}

GameResponse game_level_loop(void)
{
	char	row = 0, warm = 0;
	sun_count	= 500;

	char	joymove = 0;
	bool	joydown = false;
	char	step = 1;
	char	countdown = 5;
	
	for(;;)
	{
		char sirq = rirq_count;

		for(char i=0; i<step; i++)
		{
			if (row & 1)
			{
				if (zombies_advance(row >> 1))
					return GMENU_FAILED;
			}
			else
				plants_iterate(row >> 1);

			row++;
			if (row == 10)
			{
				row = 0;
				level_iterate();
				if (level_complete() && zombies_done())
				{
					countdown--;
					if (countdown == 0)
						return GMENU_COMPLETED;
				}
			}

		}

		warm += step;
		if (warm >= 25)
		{
			warm -= 25;
			menu_warmup();
		}

		shots_advance(step);

		if (!mower_advance())
			sun_advance();

		if (!(level->flags & LF_CONVEYOR) && !(level->flags & LF_NIGHT))
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
			case KSCAN_D | KSCAN_QUAL_DOWN:
				cursor_move(1, 0);
				break;
			case KSCAN_CSR_RIGHT | KSCAN_QUAL_SHIFT | KSCAN_QUAL_DOWN:
			case KSCAN_A | KSCAN_QUAL_DOWN:
				cursor_move(-1, 0);
				break;
			case KSCAN_CSR_DOWN | KSCAN_QUAL_DOWN:
			case KSCAN_S | KSCAN_QUAL_DOWN:
				cursor_move(0, 1);
				break;
			case KSCAN_CSR_DOWN | KSCAN_QUAL_SHIFT | KSCAN_QUAL_DOWN:
			case KSCAN_W | KSCAN_QUAL_DOWN:
				cursor_move(0, -1);
				break;
			case KSCAN_Q | KSCAN_QUAL_DOWN:
				menu_move(-1);
				break;
			case KSCAN_E | KSCAN_QUAL_DOWN:
				menu_move(1);
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
			case KSCAN_RETURN | KSCAN_QUAL_DOWN:
				cursor_select();
				break;
			case KSCAN_STOP | KSCAN_QUAL_DOWN:
				{
					GameResponse	gr = gamemenu_ingame();
					plant_grid_draw();
					if (gr != GMENU_CONTINUE)
						return gr;
					sirq = rirq_count;
				}
				break;
		}

		joy_poll(0);
		if (joydown)
		{
			if (!joyb[0])
			{
				cursor_select();
				joydown = false;
			}
		}
		else if (joyb[0])
			joydown = true;

		if (joymove)
		{
			joymove--;
			if (!joyx[0] && !joyy[0])
				joymove = 0;
		}
		else if (joyx[0] || joyy[0])
		{
			if (joydown)
				menu_move(joyx[0]);
			else
				cursor_move(joyx[0], joyy[0]);
			joymove = 20;
		}

		if (sirq == rirq_count)
		{
			if (row >= 5)
				plants_animate(row - 5);
			else
				plants_animate(row);
		}

		while (sirq == rirq_count)
			;

		step = rirq_count - sirq;
	}
}

const char * level_menu[] = {
	"DAY",
	"NIGHT",
	"NIGHTMARE",
	nullptr
};

void game_loop(char li)
{
	GameResponse gr;

	for(;;)
	{
		sun_init();
		shots_init();
		zombies_init();

		gr = level_start(li);
		
		if (gr == GMENU_START)
		{
			text_put(12, 10, VCOL_ORANGE, level->name);

			text_put(11, 14, VCOL_ORANGE, P"READY...");
			for(int i=0; i<40; i++)
				vic_waitFrame();
			plant_row_draw(2);

			text_put(12, 14, VCOL_ORANGE, P"SET...");
			for(int i=0; i<40; i++)
				vic_waitFrame();
			plant_row_draw(2);

			text_put(11, 14, VCOL_ORANGE, P"PLANT!");
			for(int i=0; i<40; i++)
				vic_waitFrame();
			plant_row_draw(2);
			plant_row_draw(1);

			menu_set(1);
			cursor_show(0, 2);
			cursor_move(0, 0);

			music_patch_voice3(false);
			gr = game_level_loop();
			music_patch_voice3(true);

			spr_show(6, false);
			spr_show(7, false);
		}

		switch (gr)
		{
		case GMENU_FAILED:
			text_put( 9, 10, VCOL_GREEN, P"THE ZOMBIES");
			text_put(11, 12, VCOL_GREEN,  P"ATE YOUR");
			text_put(13, 14, VCOL_GREEN,   P"BRAINS");

			music_init(TUNE_LOST);
			for(int i=0; i<240; i++)
				vic_waitFrame();			
			break;
		case GMENU_COMPLETED:
			music_init(TUNE_VICTORY);
			li++;
			for(int i=0; i<240; i++)
				vic_waitFrame();			
			break;
		case GMENU_EXIT:
			return;

		case GMENU_RESTART:
			break;
		}

		music_active = false;

		zombies_clear();
	}
}

int main(void)
{
	display_init();

	for(;;)
	{
		back_color = VCOL_GREEN << 4;
		back_tile = PT_NONE_DAY_0;

		plant_draw_borders();
		menu_init(0);
		mower_init();
		plant_grid_clear(0b11111);

		plant_grid[0][3].type = PT_SUNFLOWER_0;
		plant_grid[0][0].type = PT_SUNFLOWER_1;
		plant_grid[1][1].type = PT_SUNFLOWER_1;
		plant_grid[1][5].type = PT_SUNFLOWER_0;
		plant_grid[2][2].type = PT_SUNFLOWER_0;
		plant_grid[3][3].type = PT_SUNFLOWER_1;
		plant_grid[4][2].type = PT_SUNFLOWER_0;
		plant_grid[4][5].type = PT_SUNFLOWER_1;

		plant_grid[1][8].type = PT_TOMBSTONE;
		plant_grid[2][5].type = PT_TOMBSTONE;
		plant_grid[3][7].type = PT_TOMBSTONE;
		plant_grid[3][9].type = PT_TOMBSTONE;

		plant_grid_draw();

		for(char i=0; i<10; i++)
			disp_put_tile(PT_FENCE, 4 * i, 0);

		text_put_2( 3, 1, VCOL_LT_GREEN, VCOL_GREEN, P"VEGGIES");
		text_put( 18, 2, VCOL_WHITE, P"VS");
		text_put_2( 23, 1, VCOL_LT_GREY, VCOL_MED_GREY, P"UNDEAD");

		text_put( 5, 18, VCOL_LT_BLUE, P"C64 CONVERSION");
		text_put( 4, 19, VCOL_LT_GREY, P"DR.MORTAL WOMBAT");
		text_put( 15, 21, VCOL_LT_BLUE, P"MUSIC");
		text_put( 2, 22, VCOL_LT_GREY, P"ARRANGED BY CRISPS");
		text_put( 5, 23, VCOL_LT_GREY, P"OST BY LAURA S.");

		music_init(TUNE_TITLE);
		music_active = true;

		char	ls = gamemenu_query(level_menu);


		char li = 0;

		switch (ls)
		{
		case 0:
			li = 0;
			break;
		case 1:
			li = 9;
			break;
		case 2:
			li = 18;
			break;
		}

		game_loop(li);
	}

	return 0;
}
