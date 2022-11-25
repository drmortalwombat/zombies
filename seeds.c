#include "seeds.h"
#include <c64/joystick.h>
#include <c64/keyboard.h>
#include "display.h"
#include "gamemusic.h"

const const SeedInfo seed_info[16] = {
	[SD_SUNFLOWER] =     {PT_SUNFLOWER_0,     50, 17, true},
	[SD_SUNSHROOM] =     {PT_SUNSHROOM_0,     25, 17, true},
	[SD_PUFFSHROOM] =    {PT_PUFFSHROOM_0,     0, 17, true},
	[SD_PEASHOOTER] =    {PT_PEASHOOTER_0,   100, 17, true},

	[SD_SNOWPEA] =       {PT_SNOWPEA_0,      175, 17},
	[SD_WALLNUT] =       {PT_WALLNUT_0,       50,  6},
	[SD_POTATOMINE] =    {PT_POTATOMINE_0,    25,  6},
	[SD_REPEATER] =      {PT_REPEATER_0,     200, 17},

	[SD_CHERRYBOMB] =    {PT_CHERRYBOMB,     150,  3},
	[SD_CHOMPER] =       {PT_CHOMPER_0,      150, 17},
	[SD_FUMESHROOM] =    {PT_FUMESHROOM_0,    75, 17, true},
	[SD_SCAREDYSHROOM] = {PT_SCAREDYSHROOM_0, 25, 17, true},

	[SD_GRAVEDIGGER] =   {PT_GRAVEDIGGER,     75, 17, true}
};

void disp_put_char(PlantType type, char sx, char sy, char dx, char dy)
{
	__assume(dx < 40);
	__assume(dy < 25);
	__assume(sx < 4);
	__assume(sy < 4);

	char * hdp = Hires + 8 * dx + 320 * dy;
	const char * sdp = PlantsHiresData + 8 * 16 * type + 8 * sx + 32 * sy;

	for(char j=0; j<8; j++)
		hdp[j] = sdp[j];

	char * cdp = Color + 40 * dy;
	hdp = Screen + 40 * dy;

	const char * scdp = PlantsColor0Data + 16 * type + 4 * sy;
	const char * shdp = PlantsColor1Data + 16 * type + 4 * sy;

	cdp[dx] = scdp[sx];
	hdp[dx] = shdp[sx];
}

void disp_put_tile(PlantType type, char dx, char dy)
{
	__assume(dx < 36);
	__assume(dy < 21);

	char * hdp = Hires + 8 * dx + 320 * dy;
	const char * sdp = PlantsHiresData + 8 * 16 * type;

	for(char i=0; i<4; i++)
	{
		for(char j=0; j<32; j++)
			hdp[j] = sdp[j];
		hdp += 320;
		sdp += 32;
	}

	char * cdp = Color + 40 * dy + dx;
	hdp = Screen + 40 * dy + dx;

	const char * scdp = PlantsColor0Data + 16 * type;
	const char * shdp = PlantsColor1Data + 16 * type;

	for(char i=0; i<4; i++)
	{
		for(char j=0; j<4; j++)
		{
			cdp[j] = scdp[j];
			hdp[j] = shdp[j];			
		}
		cdp += 40;
		hdp += 40;
		scdp += 4;
		shdp += 4;
	}
}

void disp_ghost_tile(PlantType type, char dx, char dy)
{
	__assume(dx < 36);
	__assume(dy < 21);

	char * cdp = Color + 40 * dy + dx;
	char * hdp = Screen + 40 * dy + dx;

	for(char i=0; i<4; i++)
	{
		#pragma unroll(full)
		for(char j=0; j<4; j++)
		{
			cdp[j] = color_grey[PlantsColor0Data[16 * type + 4 * i + j] & 0x0f];
			char c = PlantsColor1Data[16 * type + 4 * i + j];
			hdp[j] = color_grey[c & 0x0f] | (color_grey[c >> 4] << 4);
		}
		cdp += 40;
		hdp += 40;
	}	
}

void disp_color_tile(PlantType type, char dx, char dy)
{
	__assume(dx < 36);
	__assume(dy < 21);

	char * cdp = Color + 40 * dy + dx;
	char * hdp = Screen + 40 * dy + dx;

	for(char i=0; i<4; i++)
	{
		#pragma unroll(full)
		for(char j=0; j<4; j++)
		{
			cdp[j] = PlantsColor0Data[16 * type + 4 * i + j];
			char c = PlantsColor1Data[16 * type + 4 * i + j];
			hdp[j] = c;
		}
		cdp += 40;
		hdp += 40;
	}	
}

void disp_put_price(unsigned v, char dx, char dy)
{
	char	c[4], c0 = 10;

	if (v >= 1000)
	{
		char	p = 0;
		while (v >= 1000)
		{
			p ++;
			v -= 1000;
		}
		c[0] = p;
		c0 = 0;
	}
	else
		c[0] = c0;

	if (v >= 100)
	{
		char	p = 0;
		while (v >= 100)
		{
			p ++;
			v -= 100;
		}
		c[1] = p;
		c0 = 0;
	}
	else
		c[1] = c0;

	if (v >= 10)
	{
		char	p = 0;
		while (v >= 10)
		{
			p ++;
			v -= 10;
		}
		c[2] = p;
		c0 = 0;
	}
	else
		c[2] = c0;

	c[3] = v;

	char * hdp = Hires + 320 * dy + 8 * dx;
	char * cdp = Screen + 40 * dy + dx;

	for(char i=0; i<4; i++)
	{
		const char * sdp = DigitsHiresData + 8 * c[i];
		cdp[i] = VCOL_LT_GREY;

		for(char j=0; j<8; j++)
			hdp[j] = sdp[j];
		hdp += 8;
	}

}

void disp_put_noprice(char dx, char dy)
{
	char * hdp = Hires + 320 * dy + 8 * dx;
	char * cdp = Screen + 40 * dy + dx;

	const char * sdp = DigitsHiresData + 80;

	for(char i=0; i<4; i++)
	{
		cdp[i] = VCOL_LT_GREY;

		for(char j=0; j<8; j++)
			hdp[j] = sdp[j];
		hdp += 8;
	}

}


void seeds_edit_open(void)
{
	disp_put_char(PT_BORDER, 0, 0, 7, 6);
	disp_put_char(PT_BORDER, 0, 2, 7, 22);

	for(char x=0; x<24; x++)
	{
		disp_put_char(PT_BORDER, 1, 0, 8 + x, 6);
		disp_put_char(PT_BORDER, 1, 2, 8 + x, 22);
	}
	for(char y=0; y<15; y++)
	{
		disp_put_char(PT_BORDER, 0, 1, 7, 7 + y);
		disp_put_char(PT_BORDER, 2, 1, 32, 7 + y);
	}

	disp_put_char(PT_BORDER, 2, 0, 32, 6);
	disp_put_char(PT_BORDER, 2, 2, 32, 22);
}

void seeds_edit_menu(SeedFlags seeds)
{
	music_init(TUNE_SEEDS);
	music_active = true;

	seeds_edit_open();

	Seeds		seedtab[16];
	unsigned	seedused = 0;

	char n = 0;
	for(char i=0; i<15; i++)
	{
		if (seeds & (1u << i))
		{
			disp_put_tile(seed_info[i].plant, 8 + 4 * (n % 6), 7 + 5 * (n / 6));
			disp_put_price(seed_info[i].cost, 8 + 4 * (n % 6), 11 + 5 * (n / 6));
			seedtab[n] = i;
			n++;
		}
	}

	char numSeeds = n;

	while (n < 17)
	{
		disp_put_tile(PT_CARDSLOT, 8 + 4 * (n % 6), 7 + 5 * (n / 6));
		disp_put_noprice(8 + 4 * (n % 6), 11 + 5 * (n / 6));
		n++;		
	}

	disp_put_tile(PT_GO, 8 + 4 * (n % 6), 7 + 5 * (n / 6));
	disp_put_noprice(8 + 4 * (n % 6), 11 + 5 * (n / 6));
	n++;		

	spr_set(6, true, 0, 0, 16 + 108, VCOL_WHITE, false, true, true);

	char	mx = 0, my = 0;
	for(;;)
	{
		char		y = 50 + 8 * 7 + my * 40;
		unsigned	x = 24 + 8 * 8 + mx * 32;

		rirq_data(&cursorMux, 0, y);
		rirq_data(&cursorMux, 1, x);
		rirq_data(&cursorMux, 2, (x & 0x100) != 0 ? 0x40 : 0x00);
		rirq_data(&cursorMux, 3, 16 + 108);
		zombies_set_msbx(0x40, (x & 0x100) != 0 ? 0x40 : 0x00);

		keyb_poll();
		switch (keyb_key)
		{
			case KSCAN_CSR_RIGHT | KSCAN_QUAL_DOWN:
				if (mx < 5)
					mx++;
				break;
			case KSCAN_CSR_RIGHT | KSCAN_QUAL_SHIFT | KSCAN_QUAL_DOWN:
				if (mx > 0)
					mx--;
				break;
			case KSCAN_CSR_DOWN | KSCAN_QUAL_DOWN:
				if (my < 2)
					my++;
				break;
			case KSCAN_CSR_DOWN | KSCAN_QUAL_SHIFT | KSCAN_QUAL_DOWN:
				if (my > 0)
					my--;
				break;
			case KSCAN_SPACE | KSCAN_QUAL_DOWN:
				{
					char si = my * 6 + mx;
					if (si == 17)
						return;

					if (si < numSeeds)
					{
						Seeds	s = seedtab[si];
						if (seedused & (1 << si))
						{
							seedused &= ~(1 << si);
							disp_color_tile(seed_info[s].plant, 8 + 4 * mx, 7 + 5 * my);
							menu_remove_item(seed_info[s].plant);
						}
						else if (menu_size < 7)
						{
							seedused |= 1 << si;
							disp_ghost_tile(seed_info[s].plant, 8 + 4 * mx, 7 + 5 * my);
							menu_add_item(seed_info[s].plant, seed_info[s].cost, seed_info[s].warm, seed_info[s].ready, false);;
						}
					}
				}
				break;
		}

		vic_waitFrame();
	}
}
