#include "plants.h"
#include "display.h"
#include "zombies.h"
#include <c64/vic.h>
#include <audio/sidfx.h>

const char PlantsHiresData[] = {
	#embed ctm_chars "plants.ctm"
};

const char PlantsColor0Data[] = {
	#embed ctm_attr1 "plants.ctm"
};

const char PlantsColor1Data[] = {
	#embed ctm_attr2 "plants.ctm"
};

#pragma align(PlantsHiresData, 256)
#pragma align(PlantsColor0Data, 256)
#pragma align(PlantsColor1Data, 256)

Plant	plant_grid[5][10];
char	plant_first[5];

int 		sun_x, sun_y, sun_vx, sun_vy;
bool		sun_active;
char		sun_power;
char		back_color;

SIDFX	SIDFXZombieHit[1] = {{
	4000, 4096,
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_168,
	-400, 0,
	2, 6,
	10
}};

SIDFX	SIDFXPlanted[1] = {{
	20000, 4096,
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_16 | SID_DKY_6,
	0xf0  | SID_DKY_240,
	-400, 0,
	2, 16,
	20
}};

SIDFX	SIDFXExplosion[1] = {{
	1000, 1000, 
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_1500,
	-20, 0,
	8, 40,
	30
}};


__striped int HiresOffset[10] = {
	16 + 0 * 32,
	16 + 1 * 32,
	16 + 2 * 32,
	16 + 3 * 32,
	16 + 4 * 32,
	16 + 5 * 32,
	16 + 6 * 32,
	16 + 7 * 32,
	16 + 8 * 32,
	16 + 9 * 32,
};

static inline char plant_color_map(char c)
{
	return ((c & 0xf0) == (VCOL_PURPLE << 4)) ? (c & 0x0f) | back_color : c;
}


void plant_row_draw(char y)
{
	for(char x=0; x<9; x++)
		plant_draw(x, y);	
}

void plant_grid_draw(void)
{
	for(char y=0; y<5; y++)
		for(char x=0; x<9; x++)
			plant_draw(x, y);
}

void plant_grid_clear(char rows)
{
	for(char y=0; y<5; y++)
	{
		char	t = (rows & 1) ? back_tile : PT_GROUND;
		rows >>= 1;
		for(char x=0; x<10; x++)
			plant_grid[y][x].type = t;
		plant_first[y] = 0xff;
	}
}

void plant_place(char x, char y, PlantType p)
{
	__assume(x < 10);

	__assume(y < 5);
	Plant	*	pp = plant_grid[y] + x;

	PlantType	pt = pp->type;

	pp->type = p;
	pp->age = 0;


	switch (p)
	{
		case PT_SUNFLOWER_0:
		case PT_SUNFLOWER_1:
		case PT_SUNSHROOM_0:
		case PT_SUNSHROOM_1:
			pp->cool = 20;
			pp->live = 5;
			break;
		case PT_SNOWPEA_0:
		case PT_SNOWPEA_1:
		case PT_PEASHOOTER_0:
		case PT_PEASHOOTER_1:
		case PT_REPEATER_0:
		case PT_REPEATER_1:
			pp->cool = 3;
			pp->live = 5;
			break;
		case PT_WALLNUT_0:
		case PT_WALLNUT_1:
			pp->cool = 0;
			pp->live = 50;
			break;
		case PT_POTATOMINE_0:
		case PT_POTATOMINE_1:
			pp->type = PT_POTATOMINE_HIDDEN;
			pp->cool = 32;
			pp->live = 5;
			break;
		case PT_CHERRYBOMB:
		case PT_ICESHROOM:
		case PT_DOOMSHROOM:
			pp->cool = 3;
			pp->live = 20;
			break;
		case PT_CHOMPER_0:
		case PT_CHOMPER_1:
			pp->cool = 0;
			pp->live = 5;
			break;
		case PT_PUFFSHROOM_0:
		case PT_PUFFSHROOM_1:
		case PT_FUMESHROOM_0:
		case PT_FUMESHROOM_1:
		case PT_SCAREDYSHROOM_0:
		case PT_SCAREDYSHROOM_1:
			pp->cool = 3;
			pp->live = 5;
			break;			
		case PT_GRAVEDIGGER:
			pp->type = PT_GRAVEDIGGER_0;
			pp->cool = 20;
			pp->live = 5;
			break;
	}

	if (pt < PT_TOMBSTONE)
	{
		char i = plant_first[y];
		if (i > x)
		{
			plant_first[y] = x;
		}
		else
		{
			char pi;
			do 
			{
				__assume(i < 10);
				pi = i;
				i = plant_grid[y][i].next;
			} while (i < x);
			plant_grid[y][pi].next = x;
		}

		pp->next = i;
	}

	sidfx_play(2, SIDFXPlanted, 1);
}

void plant_remove(char x, char y)
{
	__assume(x < 10);

	plant_grid[y][x].type = back_tile;

	char i = plant_first[y];
	if (i == x)
	{
		plant_first[y] = plant_grid[y][x].next;
	}
	else
	{
		char pi;
		do 
		{
			__assume(i < 10);
			pi = i;
			i = plant_grid[y][i].next;
		} while (i != x);
		plant_grid[y][pi].next = plant_grid[y][x].next;
	}
}

void menu_draw(char x, char t)
{
	char * hdp = Hires + 32 * x;
	const char * sdp = PlantsHiresData + 8 * 16 * t;

	for(char i=0; i<4; i++)
	{
		for(signed char j=31; j>=0; j--)
			hdp[j] = sdp[j];
		hdp += 320;
		sdp += 8 * 4;
	}

	char * cdp = Color + 4 * x;
	hdp = Screen + 4 * x;

	for(char i=0; i<4; i++)
	{
		#pragma unroll(full)
		for(char j=0; j<4; j++)
		{
			cdp[j] = PlantsColor0Data[16 * t + 4 * i + j];
			hdp[j] = plant_color_map(PlantsColor1Data[16 * t + 4 * i + j]);
		}
		cdp += 40;
		hdp += 40;
	}	
}

const char color_grey[16] = {
	VCOL_BLACK,
	VCOL_WHITE,
	VCOL_DARK_GREY,
	VCOL_LT_GREY,
	VCOL_MED_GREY,
	VCOL_MED_GREY,
	VCOL_DARK_GREY,
	VCOL_LT_GREY,

	VCOL_MED_GREY,
	VCOL_DARK_GREY,
	VCOL_MED_GREY,
	VCOL_DARK_GREY,
	VCOL_MED_GREY,
	VCOL_LT_GREY,
	VCOL_MED_GREY,
	VCOL_LT_GREY
};

void menu_cooldown(char x)
{
	if (menu[x].once)
	{
		menu[x].type = PT_CONVEYOR;
		menu_draw(x, PT_CONVEYOR);
	}
	else
	{
		char	t = menu[x].type;
		menu[x].cool = 0xff;

		char * cdp = Color + 4 * x;
		char * hdp = Screen + 4 * x;

		for(char i=0; i<4; i++)
		{
			#pragma unroll(full)
			for(char j=0; j<4; j++)
			{
				cdp[j] = color_grey[PlantsColor0Data[16 * t + 4 * i + j] & 0x0f];
				char c = plant_color_map(PlantsColor1Data[16 * t + 4 * i + j]);
				hdp[j] = color_grey[c & 0x0f] | (color_grey[c >> 4] << 4);
			}
			cdp += 40;
			hdp += 40;
		}	
	}
}


void menu_draw_color_line(char x, char y)
{
	char	t = menu[x].type;

	char * cdp = Color + 4 * x + 40 * y;
	char * hdp = Screen + 4 * x + 40 * y;

	for(char j=0; j<4; j++)
	{
		cdp[j] = PlantsColor0Data[16 * t + 4 * y + j];
		hdp[j] = plant_color_map(PlantsColor1Data[16 * t + 4 * y + j]);
	}
}

void menu_progress(char p, char m)
{
	char * hdp = Screen + 4 * 40;

	p = (m - p) * 40 / m;

	for(char j=0; j<p; j++)
	{
		hdp[j] = VCOL_LT_GREY | (VCOL_DARK_GREY << 4);
	}
	for(char j=p; j<40; j++)
	{
		hdp[j] = VCOL_YELLOW | (VCOL_ORANGE << 4);		
	}
}

void menu_init(char slots)
{
	for(char i=0; i<slots; i++)
		menu_draw(i, PT_CARDSLOT);
	for(char i=slots; i<10; i++)
		menu_draw(i, PT_FREESLOT);
	menu_size = 0;
	menu_first = 0;
}

void menu_add_item_at(char x, PlantType type, unsigned price, char warm, bool ready, bool once)
{
	menu[x].type = type;
	menu[x].price = price;
	menu[x].warm = warm;
	menu[x].cool = 0;
	menu[x].once = false;
	menu_draw(x, type);
	if (!ready)
		menu_cooldown(x);	
	menu[x].once = once;
	disp_put_price(price, 4 * x, 4);
}

void menu_add_item(PlantType type, unsigned price, char warm, bool ready, bool once)
{
	menu[menu_size].type = type;
	menu[menu_size].price = price;
	menu[menu_size].warm = warm;
	menu[menu_size].cool = 0;
	menu[menu_size].once = once;
	menu_draw(menu_size, type);
	disp_put_price(price, 4 * menu_size, 4);
	if (!ready)
		menu_cooldown(menu_size);
	if (type == PT_SUN)
		menu_first++;
	menu_size++;
}

void menu_remove_item(PlantType type)
{
	char	x = 0;
	while (x < menu_size && menu[x].type != type)
		x++;
	if (x < menu_size)
	{
		menu_size--;
		while (x < menu_size)
		{
			menu[x].type = menu[x + 1].type;
			menu[x].price = menu[x + 1].price;
			menu[x].warm = menu[x + 1].warm;
			menu[x].cool = menu[x + 1].cool;
			menu[x].once = false;
			menu_draw(x, menu[x].type);
			if (menu[x].cool != 0)
				menu_cooldown(x);	
			menu[x].once = menu[x + 1].once;
			disp_put_price(menu[x].price, 4 * x, 4);
			x++;			
		}
		menu_draw(x, PT_CARDSLOT);
		disp_put_noprice(4 * x, 4);
	}
}

void menu_warmup(void)
{
	for(char i=menu_first; i<menu_size; i++)
	{
		if (menu[i].cool)
		{
			if (menu[i].cool > menu[i].warm)
			{
				char	c = menu[i].cool - menu[i].warm;
				if ((c ^ menu[i].cool) & 0xc0)
				{
					menu_draw_color_line(i, (c >> 6) + 1);
				}
				menu[i].cool = c;
			}
			else
			{
				menu[i].cool = 0;
				menu_draw_color_line(i, 0);
			}
		}
	}
}

void sun_init(void)
{
	sun_active = false;
	spr_show(7, false);
}

void sun_advance(void)
{
	if (sun_active)
	{
		if (sun_vy < 0)
			sun_vx -= (sun_x - 32) >> 3;
		sun_vy -= (sun_y - 50) >> 3;
		sun_x += sun_vx >> 4;
		sun_y += sun_vy >> 4;
		if (sun_y < 50 || sun_x < 0)
		{
			sun_active = false;
			spr_show(7, false);
			menu[0].price += sun_power;
			disp_put_price(menu[0].price, 0, 4);
		}
		else
		{
			spr_set(7, true, sun_x, sun_y, 16 + 111, VCOL_YELLOW, true, false, false);
			zombies_set_msbx(0x80, (sun_x & 0x100) >> 1);
		}
	}
}

SIDFX	SIDFXSunshine[4] = {{
	NOTE_C(8), 4096,
	SID_CTRL_GATE | SID_CTRL_TRI,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_6,
	0, 0,
	1, 0,
	5
},{
	NOTE_E(8), 4096,
	SID_CTRL_GATE | SID_CTRL_TRI,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_6,
	0, 0,
	1, 0,
	5
},{
	NOTE_G(8), 4096,
	SID_CTRL_GATE | SID_CTRL_TRI,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_6,
	0, 0,
	1, 0,
	5
},{
	NOTE_C(9), 4096,
	SID_CTRL_GATE | SID_CTRL_TRI,
	SID_ATK_16 | SID_DKY_6,
	0xf0  | SID_DKY_168,
	0, 0,
	4, 16,
	5
}};

void sun_add(char x, char y, char vy, char power)
{
	sun_active = true;
	sun_x = 32 + 32 * x;
	sun_y = y;
	sun_vx = 0;
	sun_vy = vy;
	sun_power = power;

	sidfx_play(2, SIDFXSunshine, 4);
}

void plant_clear_mower(char y)
{
	char * hdp = Hires + 5 * 320 + 320 * 4 * y;
	const char * sdp = PlantsHiresData + 8 * 16 * PT_FLOORSPACE;

	for(char i=0; i<4; i++)
	{
		for(char j=0; j<8*2; j++)
			hdp[j] = sdp[j + 16];
		hdp += 320;
		sdp += 8 * 4;
	}	
}

void plant_draw_borders(void)
{
	for(char y=0; y<5; y++)
	{
		char * hdp = Hires + 5 * 320 + 320 * 4 * y;
		const char * sdp = PlantsHiresData + 8 * 16 * PT_FLOORSPACE_MOWER;

		for(char i=0; i<4; i++)
		{
			for(char j=0; j<8*2; j++)
				hdp[j] = sdp[j + 16];
			for(char j=0; j<8*2; j++)
				hdp[j + 304] = sdp[j];
			hdp += 320;
			sdp += 8 * 4;
		}

		char * cdp = Color + 5 * 40  + 40 * 4 * y;
		hdp = Screen + 5 * 40 + 40 * 4 * y;

		for(char i=0; i<4; i++)
		{
			#pragma unroll(full)
			for(char j=0; j<2; j++)
			{
				cdp[j] = PlantsColor0Data[16 * PT_FLOORSPACE_MOWER + 4 * i + j + 2];
				hdp[j] = plant_color_map(PlantsColor1Data[16 * PT_FLOORSPACE_MOWER + 4 * i + j + 2]);
				cdp[j + 38] = PlantsColor0Data[16 * PT_FLOORSPACE_MOWER + 4 * i + j];
				hdp[j + 38] = plant_color_map(PlantsColor1Data[16 * PT_FLOORSPACE_MOWER + 4 * i + j]);
			}
			cdp += 40;
			hdp += 40;
		}
	}

	char * hdp = Hires + 4 * 320;
	for(char i=0; i<40; i++)
	{
		const char * sdp = DigitsHiresData + 10 * 8;

		for(char j=0; j<8; j++)
			hdp[j] = sdp[j];
		hdp += 8;
	}

}

void plant_draw(char x, char y)
{
	__assume(x < 10);

	PlantType	p = plant_grid[y][x].type;

	char * hdp = HiresTab[5 + 4 * y] + HiresOffset[x];

//	Hires + 16 + 5 * 320 + 32 * x + 320 * 4 * y;
	const char * sdp = PlantsHiresData + 8 * 16 * p;

	for(char i=0; i<4; i++)
	{
		for(char j=0; j<8*4; j++)
			hdp[j] = sdp[j];
		hdp += 320;
		sdp += 8 * 4;
	}

	char * cdp = ColorTab[5 + 4 * y] + 2 + 4 * x;
	hdp = ScreenTab[5 + 4 * y] + 2 + 4 * x;

//	char * cdp = Color + 2 + 5 * 40 + 4 * x + 40 * 4 * y;
//	hdp = Screen + 2 + 5 * 40 + 4 * x + 40 * 4 * y;

	const char * scdp = PlantsColor0Data + 16 * p;
	const char * shdp = PlantsColor1Data + 16 * p;

	for(char i=0; i<4; i++)
	{
		for(char j=0; j<4; j++)
		{
			cdp[j] = scdp[j];
			hdp[j] = plant_color_map(shdp[j]);
		}
		scdp += 4;
		shdp += 4;
		
		cdp += 40;
		hdp += 40;
	}
}


void plant_draw_field(char x, char y)
{
	__assume(y < 20);
	__assume(x < 40);

	char * dp = HiresTab[y + 5] + x * 8 + 16;
	char * cp = ColorTab[y + 5] + x + 2;
	PlantType p = x >= 36 ? PT_FLOORSPACE : plant_grid[y >> 2][x >> 2].type;

	const char * sp = PlantsHiresData + 8 * 16 * p + 8 * (((y & 3) << 2) | (x & 3));

	for(char i=0; i<8; i++)
		dp[i] = sp[i];
	cp[0] = PlantsColor0Data[16 * p + (((y & 3) << 2) | (x & 3))];
}

void shots_init(void)
{
	shots_first = 0xff;
	for(char i=0; i<31; i++)
		shots[i].next = i + 1;
	shots[31].next = 0xff;
	shots_free = 0;
}

void shot_draw(char x, char y, char c)
{
	__assume(y < 20);

	char * dp = HiresTab[y + 5] + (x & 0xfc) * 2 + 16 + 2;
	char * cp = ColorTab[y + 5] + (x >> 2) + 2;

	switch(x & 3)
	{
		case 0:
			dp[0] &= 0xc3;
			dp[1] = 0x3c;
			dp[2] = 0x3c;
			dp[3] = 0x3c;
			dp[4] &= 0xc3;
			cp[0] = c;
			break;
		case 1:
			dp[0] &= 0xf0;
			dp[1] = (dp[1] & 0xc0) | 0x0f; dp[ 9] &= 0x3f; 
			dp[2] = (dp[2] & 0xc0) | 0x0f; dp[10] &= 0x3f; 
			dp[3] = (dp[3] & 0xc0) | 0x0f; dp[11] &= 0x3f; 
			dp[4] &= 0xf0;
			cp[0] = c;
			break;
		case 2:
			dp[0] &= 0xfc; dp[8] &= 0x3f;
			dp[1] = (dp[1] & 0xf0) | 0x03; dp[ 9] = (dp[ 9] & 0x0f) | 0xc0;
			dp[2] = (dp[2] & 0xf0) | 0x03; dp[10] = (dp[10] & 0x0f) | 0xc0;
			dp[3] = (dp[3] & 0xf0) | 0x03; dp[11] = (dp[11] & 0x0f) | 0xc0;
			dp[4] &= 0xfc; dp[12] &= 0x3f;
			cp[0] = c;
			cp[1] = c;
			break;
		case 3:
			dp[8] &= 0x0f;
			dp[1] &= 0xfc; dp[ 9] = (dp[ 9] & 0x03) | 0xf0;
			dp[2] &= 0xfc; dp[10] = (dp[10] & 0x03) | 0xf0;
			dp[3] &= 0xfc; dp[11] = (dp[11] & 0x03) | 0xf0;
			dp[12] &= 0x0f;
			cp[1] = c;
			break;
	}
}


void shot_clear(char x, char y)
{
	plant_draw_field(x >> 2, y);
	if (x & 3)
		plant_draw_field((x >> 2) + 1, y);
}

void shots_add(char x, char y, char t, ShotType type)
{
	if (shots_free != 0xff && x < 148)
	{
		if (x + t > 148)
			t = 148 - x;

		char	s = shots_free;
		shots_free = shots[s].next;
		shots[s].x = x;
		shots[s].y = y;
		shots[s].t = t;
		shots[s].type = type;
		shots[s].next = shots_first;
		shots_first = s;
	}
}

const char shot_colors[4] = {VCOL_YELLOW, VCOL_LT_BLUE, VCOL_PURPLE, VCOL_MED_GREY};

void shots_advance(char step)
{
	char	p = 0xff;
	char	s = shots_first;
	while (s != 0xff)
	{
		shot_clear(shots[s].x, shots[s].y);

		char sstep = step;
		if (shots[s].type == ST_FUME)
			sstep <<= 2;

		char n = shots[s].next;
		shots[s].x += sstep;
		char y = shots[s].y >> 2;
		char x = shots[s].x + 20;

		bool	keep = true;


		if (shots[s].t < sstep)
			keep = false;
		else
		{
			shots[s].t -= sstep;
			if (zombies_left[y] <= x && shots[s].type != ST_FUME)
			{

				char z = zombies_first[y];
				while (z != 0xff)
				{
					if (zombies[z].x <= x && zombies[z].x > x - 10 && zombies[z].live > 0)
					{
						if (shots[s].type == ST_FROST && zombies[z].type != ZOMBIE_SCREENDOOR)
							zombies[z].frozen = 3;
						zombies[z].live -= 2;
						zombies[z].frozen |= 0x80;

						keep = false;
						sidfx_play(2, SIDFXZombieHit, 1);
						break;
					}
					z = zombies[z].next;
				}
			}
		}

		if (keep)
		{
			shot_draw(shots[s].x, shots[s].y, shot_colors[shots[s].type]);
			p = s;
		}
		else
		{
			if (p == 0xff)
				shots_first = n;
			else
				shots[p].next = n;
			shots[s].next = shots_free;
			shots_free = s;
		}
		s = n;
	}
}

bool plant_scared_row(char x, char y)
{
	if (zombies_left[y] < x + 48 && zombies_right[y] + 8 >= x)
	{
		char s = zombies_first[y];
		while (s != 0xff)
		{
			if (zombies[s].x < x + 48 && zombies[s].x + 8 >= x)
				return true;
			s = zombies[s].next;
		}
	}

	return false;
}

bool plant_scared(char x, char y)
{
	x *= 16;

	return 
		plant_scared_row(x, y) || 
		y > 0 && plant_scared_row(x, y - 1) ||
		y < 4 && plant_scared_row(x, y + 1);
}

void plants_iterate(char y)
{
	char ps = 0xff;
	char s = plant_first[y];
	char right = zombies_right[y] >> 4;
	char left = zombies_left[y] >> 4;

	while (s != 0xff)
	{
		__assume(s < 10);

		Plant	*	p = plant_grid[y] + s;
		char n = p->next;

		if (p->cool)
			p->cool--;
		else
		{
			unsigned r = rand();

			switch (p->type)
			{
				case PT_PEASHOOTER_0:
				case PT_PEASHOOTER_1:
					if (s < right)
					{
						p->cool = 7 + (r & 3);
						shots_add(16 * s + 12, 4 * y + 1, 255, ST_PEA);
					}
					break;

				case PT_REPEATER_0:
				case PT_REPEATER_1:
					if (s < right)
					{
						p->cool = 7 + (r & 3);
						shots_add(16 * s + 16, 4 * y + 1, 255, ST_PEA);
						shots_add(16 * s + 8, 4 * y + 1, 255, ST_PEA);
					}
					break;

				case PT_SNOWPEA_0:
				case PT_SNOWPEA_1:
					if (s < right)
					{
						p->cool = 7 + (r & 3);
						shots_add(16 * s + 12, 4 * y + 1, 255, ST_FROST);
					}
					break;

				case PT_PUFFSHROOM_0:
				case PT_PUFFSHROOM_1:
					if (s < right && s + 3 >= left)
					{
						p->cool = 7 + (r & 3);
						shots_add(16 * s + 12, 4 * y + 2, 36, ST_PUFF);
					}
					break;

				case PT_FUMESHROOM_0:
				case PT_FUMESHROOM_1:
					if (s < right && s + 4 >= left)
					{
						p->cool = 7 + (r & 3);
						shots_add(16 * s + 12, 4 * y + 1, 48, ST_FUME);
						shots_add(16 * s + 22, 4 * y + 1, 32, ST_FUME);
						shots_add(16 * s + 18, 4 * y + 0, 42, ST_FUME);
						shots_add(16 * s + 14, 4 * y + 2, 36, ST_FUME);
						zombies_fume(16 * s + 16, y, 64);
					}
					break;

				case PT_POTATOMINE_HIDDEN:
					p->cool = 4;
					p->type = PT_POTATOMINE_0;
					plant_draw(s, y);
					break;

				case PT_GRAVEDIGGER_0:
				case PT_EXPLOSION_3:
				case PT_POTATOMINE_EXPLODED:
				case PT_SNOWFLAKE:
					p->type = back_tile;
					plant_draw(s, y);

					if (ps != 0xff)
						plant_grid[y][ps].next = n;
					else
						plant_first[y] = n;
					s = ps;
					break;

				case PT_DOOMCLOUD:
					p->type = PT_CRATER;
					plant_draw(s, y);
					break;				

				case PT_POTATOMINE_0:
				case PT_POTATOMINE_1:
					p->cool = 4;

					char z = zombies_first[y];
					char x = s * 16;
					while (z != 0xff)
					{
						if (zombies[z].x <= x + 32 && zombies[z].x > x + 8 && zombies[z].live > 0)
						{
							zombies[z].live = -80;
							p->type = PT_POTATOMINE_EXPLODED;
							p->cool = 8;
						}
						z = zombies[z].next;
					}
					if (p->type == PT_POTATOMINE_EXPLODED)
					{						
						plant_draw(s, y);			
						sidfx_play(2, SIDFXExplosion, 1);
					}
					break;

				case PT_SUNFLOWER_0:
				case PT_SUNFLOWER_1:
				case PT_SUNSHROOM_BIG_0:
				case PT_SUNSHROOM_BIG_1:
					if (!sun_active)
					{
						p->cool = 64 + (r & 63);
						sun_add(s, 50 + 8 * 5 + 32 * y, 0, 25);					
					}
					break;

				case PT_SUNSHROOM_0:
				case PT_SUNSHROOM_1:
					if (!sun_active)
					{
						p->cool = 64 + (r & 63);
						sun_add(s, 50 + 8 * 5 + 32 * y, 0, 15);					
						p->age++;
						if (p->age == 4)
						{
							p->type = PT_SUNSHROOM_BIG_0;
							plant_draw(s, y);
						}
					}
					break;

				case PT_CHOMPER_EAT_0:
				case PT_CHOMPER_EAT_1:
					p->type = PT_CHOMPER_0;
					plant_draw(s, y);
					break;

				case PT_ICESHROOM:
					p->type = PT_SNOWFLAKE;
					p->cool = 2;
					plant_draw(s, y);
					zombies_freeze_all(10);
					break;

				case PT_DOOMSHROOM:
					p->type = PT_DOOMCLOUD;
					p->cool = 2;
					plant_draw(s, y);
					{
						char sy = 0, ey = 5;
						if (y > 2)
							sy = y -2;
						else if (y < 2)
							ey = y + 3;
						for(char i=sy; i<ey; i++)
							zombies_splash(s * 16 + 28, i, 48, 127);
					}
					sidfx_play(2, SIDFXExplosion, 1);					
					break;

				case PT_CHERRYBOMB:
					zombies_splash(s * 16 + 28, y, 16, 80);
					if (y > 0)
						zombies_splash(s * 16 + 28, y - 1, 16, 80);
					if (y < 4)
						zombies_splash(s * 16 + 28, y + 1, 16, 80);
					p->type = PT_EXPLOSION_0;
					plant_draw(s, y);
					sidfx_play(2, SIDFXExplosion, 1);					
					break;					
				case PT_EXPLOSION_0:
					p->type = PT_EXPLOSION_1;
					plant_draw(s, y);
					break;
				case PT_EXPLOSION_1:
					p->type = PT_EXPLOSION_2;
					plant_draw(s, y);
					break;
				case PT_EXPLOSION_2:
					p->type = PT_EXPLOSION_3;
					plant_draw(s, y);
					break;

				case PT_SCAREDYSHROOM_0:
				case PT_SCAREDYSHROOM_1:
					if (plant_scared(s, y))
					{
						p->type = PT_SCAREDYSHROOM_DUCK;
						plant_draw(s, y);
						p->cool = 4;
					}
					else if (s < right)
					{
						p->cool = 7 + (r & 3);
						shots_add(16 * s + 12, 4 * y + 1, 255, ST_PUFF);
					}
					else
						p->cool = 4;
					break;

				case PT_SCAREDYSHROOM_DUCK:
					if (!plant_scared(s, y))
					{
						p->type = PT_SCAREDYSHROOM_0;
						plant_draw(s, y);
					}
					p->cool = 4;
					break;
			}
		}

		ps = s;
		s = n;
	}
}

PlantType	plant_anim_tab[NUM_PLANT_TYPES] = {
	[PT_SUNFLOWER_0] = PT_SUNFLOWER_1,
	[PT_SUNFLOWER_1] = PT_SUNFLOWER_0,

	[PT_REPEATER_0] = PT_REPEATER_1,
	[PT_REPEATER_1] = PT_REPEATER_0,

	[PT_PEASHOOTER_0] = PT_PEASHOOTER_1,
	[PT_PEASHOOTER_1] = PT_PEASHOOTER_0,

	[PT_WALLNUT_0] = PT_WALLNUT_1,
	[PT_WALLNUT_1] = PT_WALLNUT_0,

	[PT_CHOMPER_0] = PT_CHOMPER_1,
	[PT_CHOMPER_1] = PT_CHOMPER_0,

	[PT_CHOMPER_EAT_0] = PT_CHOMPER_EAT_1,
	[PT_CHOMPER_EAT_1] = PT_CHOMPER_EAT_0,

	[PT_POTATOMINE_0] = PT_POTATOMINE_1,
	[PT_POTATOMINE_1] = PT_POTATOMINE_0,

	[PT_SNOWPEA_0] = PT_SNOWPEA_1,
	[PT_SNOWPEA_1] = PT_SNOWPEA_0,

	[PT_PUFFSHROOM_0] = PT_PUFFSHROOM_1,
	[PT_PUFFSHROOM_1] = PT_PUFFSHROOM_0,

	[PT_SUNSHROOM_0] = PT_SUNSHROOM_1,
	[PT_SUNSHROOM_1] = PT_SUNSHROOM_0,

	[PT_SUNSHROOM_BIG_0] = PT_SUNSHROOM_BIG_1,
	[PT_SUNSHROOM_BIG_1] = PT_SUNSHROOM_BIG_0,

	[PT_FUMESHROOM_0] = PT_FUMESHROOM_1,
	[PT_FUMESHROOM_1] = PT_FUMESHROOM_0,

	[PT_SCAREDYSHROOM_0] = PT_SCAREDYSHROOM_1,
	[PT_SCAREDYSHROOM_1] = PT_SCAREDYSHROOM_0,
	
};

void plants_animate(char y)
{
	unsigned	r = rand();
	char		x = ((r & 0xff) * 9) >> 8;

	Plant	*	p = plant_grid[y] + x;
	PlantType	t = plant_anim_tab[p->type];
	if (t)
	{
		p->type = t;
		plant_draw(x, y);
		return;
	}
}
