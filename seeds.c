#include "seeds.h"
#include <c64/joystick.h>
#include <c64/keyboard.h>
#include "display.h"
#include "gamemusic.h"

const const SeedInfo seed_info[16] = {
	[SD_PEASHOOTER] =    {PT_PEASHOOTER_0,   100, 17, true},
	[SD_SUNFLOWER] =     {PT_SUNFLOWER_0,     50, 17, true},
	[SD_CHERRYBOMB] =    {PT_CHERRYBOMB,     150,  3},
	[SD_WALLNUT] =       {PT_WALLNUT_0,       50,  6},

	[SD_POTATOMINE] =    {PT_POTATOMINE_0,    25,  6},
	[SD_SNOWPEA] =       {PT_SNOWPEA_0,      175, 17},
	[SD_CHOMPER] =       {PT_CHOMPER_0,      150, 17},
	[SD_REPEATER] =      {PT_REPEATER_0,     200, 17},

	[SD_PUFFSHROOM] =    {PT_PUFFSHROOM_0,     0, 17, true},
	[SD_SUNSHROOM] =     {PT_SUNSHROOM_0,     25, 17, true},
	[SD_FUMESHROOM] =    {PT_FUMESHROOM_0,    75, 17, true},
	[SD_SCAREDYSHROOM] = {PT_SCAREDYSHROOM_0, 25, 17, true},

	[SD_GRAVEDIGGER] =   {PT_GRAVEDIGGER,     75, 17, true},
	[SD_ICESHROOM] =	 {PT_ICESHROOM,       75, 3},
	[SD_DOOMSHROOM] =	 {PT_DOOMSHROOM,	 125, 3}
};

// Draw box around seed selection menu
void seeds_edit_open(void)
{
	// Top left and right
	disp_put_char(PT_BORDER, 0, 0, 7, 6);
	disp_put_char(PT_BORDER, 0, 2, 7, 22);

	// Top and bottom border
	for(char x=0; x<24; x++)
	{
		disp_put_char(PT_BORDER, 1, 0, 8 + x, 6);
		disp_put_char(PT_BORDER, 1, 2, 8 + x, 22);
	}

	// Left and right border
	for(char y=0; y<15; y++)
	{
		disp_put_char(PT_BORDER, 0, 1, 7, 7 + y);
		disp_put_char(PT_BORDER, 2, 1, 32, 7 + y);
	}

	// Bottom left and right
	disp_put_char(PT_BORDER, 2, 0, 32, 6);
	disp_put_char(PT_BORDER, 2, 2, 32, 22);
}

// Position of seed menu slots
static const char seed_menu_x[18] = {
	8, 12, 16, 20, 24, 28,
	8, 12, 16, 20, 24, 28,
	8, 12, 16, 20, 24, 28	
};

static const char seed_menu_y[18] = {
	7, 7, 7, 7, 7, 7,
	12, 12, 12, 12, 12, 12,
	17, 17, 17, 17, 17, 17
};

GameResponse seeds_edit_menu(SeedFlags seeds, char slots)
{
	// Default response
	GameResponse	rs = GMENU_CONTINUE;

	// Show seeds selection menu
	seeds_edit_open();

	// List of seeds selected
	Seeds		seedtab[16];

	// Flag set of seeds already used in menu
	unsigned	seedused = 0;

	// Draw seeds in menu
	char n = 0;
	for(char i=0; i<15; i++)
	{
		// Check if seed is available
		if (seeds & (1u << i))
		{
			char	x = seed_menu_x[n];
			char	y = seed_menu_y[n];

			// Draw plant image and cost
			disp_put_tile(seed_info[i].plant, x, y);
			disp_put_price(seed_info[i].cost, x, y + 4);
			disp_color_price(x, y + 4);

			// Add to list of available seeds
			seedtab[n] = i;
			n++;
		}
	}

	char numSeeds = n;

	// Fill remaining slots in menu with empty
	while (n < 17)
	{
		char	x = seed_menu_x[n];
		char	y = seed_menu_y[n];

		disp_put_tile(PT_CARDSLOT, x, y);
		disp_put_noprice(x, y + 4);
		disp_color_price(x, y + 4);
		n++;		
	}

	// Add final go menu slot
	{
		char	x = seed_menu_x[n];
		char	y = seed_menu_y[n];

		disp_put_tile(PT_GO, x, y);
		disp_put_noprice(x, y + 4);
		disp_color_price(x, y + 4);
		n++;		
	}

	// Show selection sprite offscreen
	spr_set(6, true, 0, 0, 16 + 108, VCOL_WHITE, false, true, true);

	// Remember previous state of joystick
	char 	joymove = 0;
	bool	joydown = false;

	// Top left menu entry
	char	mx = 0, my = 0;

	// Repeat until exit requestd
	while (rs == GMENU_CONTINUE)
	{
		char		y = 49 + 8 * 7 + my * 40;
		unsigned	x = 23 + 8 * 8 + mx * 32;

		// Place selection box sprite
		rirq_data(&cursorMux, 0, y);
		rirq_data(&cursorMux, 1, x);
		rirq_data(&cursorMux, 2, (x & 0x100) != 0 ? 0x40 : 0x00);
		rirq_data(&cursorMux, 3, 16 + 108);
		zombies_set_msbx(0x40, (x & 0x100) != 0 ? 0x40 : 0x00);

		bool	select = false;

		keyb_poll();
		switch (keyb_key)
		{
			// Movement by keyboard, cursor or WASD
			case KSCAN_CSR_RIGHT | KSCAN_QUAL_DOWN:
			case KSCAN_D | KSCAN_QUAL_DOWN:
				if (mx < 5)
					mx++;
				break;
			case KSCAN_CSR_RIGHT | KSCAN_QUAL_SHIFT | KSCAN_QUAL_DOWN:
			case KSCAN_A | KSCAN_QUAL_DOWN:
				if (mx > 0)
					mx--;
				break;
			case KSCAN_CSR_DOWN | KSCAN_QUAL_DOWN:
			case KSCAN_S | KSCAN_QUAL_DOWN:
				if (my < 2)
					my++;
				break;
			case KSCAN_CSR_DOWN | KSCAN_QUAL_SHIFT | KSCAN_QUAL_DOWN:
			case KSCAN_W | KSCAN_QUAL_DOWN:
				if (my > 0)
					my--;
				break;

			// Selection
			case KSCAN_SPACE | KSCAN_QUAL_DOWN:
			case KSCAN_RETURN | KSCAN_QUAL_DOWN:
				select = true;
				break;
			case KSCAN_STOP | KSCAN_QUAL_DOWN:
				{
					// Open ingame menu
					rs = gamemenu_ingame();

					// Restore seeds menu
					seeds_edit_open();

					for(char i=0; i<18; i++)
					{
						char	x = seed_menu_x[i];
						char	y = seed_menu_y[i];

						if (i < numSeeds)
						{
							disp_put_tile(seed_info[i].plant, x, y);
							disp_put_price(seed_info[i].cost, x, y + 4);
							if (seedused & (1 << i))
								disp_ghost_tile(seed_info[i].plant, x, y);
						}
						else if (i < 17)
						{
							disp_put_tile(PT_CARDSLOT, x, y);
							disp_put_noprice(x, y + 4);							
						}
						else
						{
							disp_put_tile(PT_GO, x, y);
							disp_put_noprice(x, y + 4);														
						}

						disp_color_price(x, y + 4);
					}
				}
				break;
		}

		joy_poll(0);

		// Check joystick button up
		if (joydown)
		{
			if (!joyb[0])
				joydown = false;
		}
		else if (joyb[0])
		{
			// Joystick button was up and now pressed
			select = true;
			joydown = true;
		}

		// Check joystick release and movement
		if (joymove)
		{
			joymove--;
			if (!joyx[0] && !joyy[0])
				joymove = 0;
		}
		else if (joyx[0] < 0 && mx > 0)
		{
			mx--;
			joymove = 20;
		}
		else if (joyx[0] > 0 && mx < 5)
		{
			mx++;
			joymove = 20;
		}
		else if (joyy[0] < 0 && my > 0)
		{
			my--;
			joymove = 20;
		}
		else if (joyy[0] > 0 && my < 2)
		{
			my++;
			joymove = 20;
		}

		// Current seed is selected
		if (select)
		{
			// Get index of slot
			char si = my * 6 + mx;

			// Check for GO button
			if (si == 17 && menu_size == slots + 1)
				rs = GMENU_START;
			else if (si < numSeeds)
			{
				Seeds	s = seedtab[si];
				// Is seed already used
				if (seedused & (1 << si))
				{
					// Remove seed from menu
					seedused &= ~(1 << si);
					disp_color_tile(seed_info[s].plant, 8 + 4 * mx, 7 + 5 * my);
					menu_remove_item(seed_info[s].plant);
				}
				else if (menu_size < slots + 1)
				{
					// Add seed to menu
					seedused |= 1 << si;
					disp_ghost_tile(seed_info[s].plant, 8 + 4 * mx, 7 + 5 * my);
					menu_add_item(seed_info[s].plant, seed_info[s].cost, seed_info[s].warm, seed_info[s].ready, false);;
				}
			}
		}
		vic_waitFrame();
	}

	// Hide selection sprite
	spr_show(6, false);

	return rs;
}
