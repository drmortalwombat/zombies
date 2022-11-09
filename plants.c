#include "plants.h"
#include "display.h"
#include "zombies.h"
#include <c64/vic.h>

const char PlantsHiresData[] = {
	#embed ctm_chars "plants.ctm"
};

const char PlantsColor0Data[] = {
	#embed ctm_attr1 "plants.ctm"
};

const char PlantsColor1Data[] = {
	#embed ctm_attr2 "plants.ctm"
};

const char DigitsHiresData[] = {
	#embed ctm_chars "digits.ctm"
};

#pragma align(PlantsHiresData, 256)
#pragma align(PlantsColor0Data, 256)
#pragma align(PlantsColor1Data, 256)

Plant	plant_grid[5][10];
char	plant_first[5];

int 		sun_x, sun_y, sun_vx, sun_vy;
bool		sun_active;
char		sun_power;

char * HiresTab[25] = {
	Hires +  0 * 320, Hires +  1 * 320, Hires +  2 * 320, Hires +  3 * 320,
	Hires +  4 * 320, Hires +  5 * 320, Hires +  6 * 320, Hires +  7 * 320,
	Hires +  8 * 320, Hires +  9 * 320, Hires + 10 * 320, Hires + 11 * 320,
	Hires + 12 * 320, Hires + 13 * 320, Hires + 14 * 320, Hires + 15 * 320,
	Hires + 16 * 320, Hires + 17 * 320, Hires + 18 * 320, Hires + 19 * 320,
	Hires + 20 * 320, Hires + 21 * 320, Hires + 22 * 320, Hires + 23 * 320,
	Hires + 24 * 320
};

char * ColorTab[25] = {
	Color +  0 * 40, Color +  1 * 40, Color +  2 * 40, Color +  3 * 40,
	Color +  4 * 40, Color +  5 * 40, Color +  6 * 40, Color +  7 * 40,
	Color +  8 * 40, Color +  9 * 40, Color + 10 * 40, Color + 11 * 40,
	Color + 12 * 40, Color + 13 * 40, Color + 14 * 40, Color + 15 * 40,
	Color + 16 * 40, Color + 17 * 40, Color + 18 * 40, Color + 19 * 40,
	Color + 20 * 40, Color + 21 * 40, Color + 22 * 40, Color + 23 * 40,
	Color + 24 * 40
};

char * ScreenTab[25] = {
	Screen +  0 * 40, Screen +  1 * 40, Screen +  2 * 40, Screen +  3 * 40,
	Screen +  4 * 40, Screen +  5 * 40, Screen +  6 * 40, Screen +  7 * 40,
	Screen +  8 * 40, Screen +  9 * 40, Screen + 10 * 40, Screen + 11 * 40,
	Screen + 12 * 40, Screen + 13 * 40, Screen + 14 * 40, Screen + 15 * 40,
	Screen + 16 * 40, Screen + 17 * 40, Screen + 18 * 40, Screen + 19 * 40,
	Screen + 20 * 40, Screen + 21 * 40, Screen + 22 * 40, Screen + 23 * 40,
	Screen + 24 * 40
};

int HiresOffset[10] = {
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
	if ((c & 0xf0) == (VCOL_PURPLE << 4))
		return (c & 0x0f) | (VCOL_GREEN << 4);
	else
		return c;
}


void plant_grid_clear(char rows)
{
	for(char y=0; y<5; y++)
	{
		char	t = (rows & 1) ? PT_NONE_DAY : PT_GROUND;
		rows >>= 1;
		for(char x=0; x<10; x++)
			plant_grid[y][x].type = t;
		plant_first[y] = 0xff;
	}
}

void plant_place(char x, char y, PlantType p)
{
	Plant	*	pp = plant_grid[y] + x;
	pp->type = p;

	switch (p)
	{
		case PT_SUNFLOWER_0:
		case PT_SUNFLOWER_1:
			pp->cool = 20;
			pp->live = 5;
			break;
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
			pp->cool = 3;
			pp->live = 20;
			break;
		case PT_CHOMPER_0:
		case PT_CHOMPER_1:
			pp->cool = 0;
			pp->live = 5;
			break;				
	}

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
			pi = i;
			i = plant_grid[y][i].next;
		} while (i < x);
		plant_grid[y][pi].next = x;
	}

	pp->next = i;
}

void plant_remove(char x, char y)
{
	__assume(x < 10);

	plant_grid[y][x].type = PT_NONE_DAY;

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
		for(char j=0; j<8*4; j++)
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

static const char colory_grey[16] = {
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
				cdp[j] = colory_grey[PlantsColor0Data[16 * t + 4 * i + j] & 0x0f];
				char c = plant_color_map(PlantsColor1Data[16 * t + 4 * i + j]);
				hdp[j] = colory_grey[c & 0x0f] | (colory_grey[c >> 4] << 4);
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

	#pragma unroll(full)
	for(char j=0; j<4; j++)
	{
		cdp[j] = PlantsColor0Data[16 * t + 4 * y + j];
		hdp[j] = plant_color_map(PlantsColor1Data[16 * t + 4 * y + j]);
	}
}

void menu_draw_price(char x, unsigned v)
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

	char * hdp = Hires + 4 * 320 + 32 * x;
	for(char i=0; i<4; i++)
	{
		const char * sdp = DigitsHiresData + 8 * c[i];

		for(char j=0; j<8; j++)
			hdp[j] = sdp[j];
		hdp += 8;
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

void menu_init(void)
{
	memset(Hires, 0, 320 * 4);
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
	menu_draw_price(x, price);
}

void menu_add_item(PlantType type, unsigned price, char warm, bool ready, bool once)
{
	menu[menu_size].type = type;
	menu[menu_size].price = price;
	menu[menu_size].warm = warm;
	menu[menu_size].cool = 0;
	menu[menu_size].once = once;
	menu_draw(menu_size, type);
	menu_draw_price(menu_size, price);
	if (!ready)
		menu_cooldown(menu_size);
	if (type == PT_SUN)
		menu_first++;
	menu_size++;
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
			menu_draw_price(0, menu[0].price);
		}
		else
		{
			spr_set(7, true, sun_x, sun_y, 16 + 111, VCOL_YELLOW, true, false, false);
			zombies_set_msbx(0x80, (sun_x & 0x100) >> 1);
		}
	}
}

void sun_add(char x, char y, char vy, char power)
{
	sun_active = true;
	sun_x = 32 + 32 * x;
	sun_y = y;
	sun_vx = 0;
	sun_vy = vy;
	sun_power = power;
}

void plant_draw_borders(void)
{
	for(char y=0; y<5; y++)
	{
		char * hdp = Hires + 5 * 320 + 320 * 4 * y;
		const char * sdp = PlantsHiresData + 8 * 16 * PT_FLOORSPACE;

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
				cdp[j] = PlantsColor0Data[16 * PT_FLOORSPACE + 4 * i + j + 2];
				hdp[j] = plant_color_map(PlantsColor1Data[16 * PT_FLOORSPACE + 4 * i + j + 2]);
				cdp[j + 38] = PlantsColor0Data[16 * PT_FLOORSPACE + 4 * i + j];
				hdp[j + 38] = plant_color_map(PlantsColor1Data[16 * PT_FLOORSPACE + 4 * i + j]);
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

	const char * sp = PlantsHiresData + 8 * 16 * p + 32 * (y & 3) + 8 * (x & 3);

	for(char i=0; i<8; i++)
		dp[i] = sp[i];
	cp[0] = PlantsColor0Data[16 * p + 4 * (y & 3) + (x & 3)];
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
			break;
		case 1:
			dp[0] &= 0xf0;
			dp[1] = (dp[1] & 0xc0) | 0x0f; dp[ 9] &= 0x3f; 
			dp[2] = (dp[2] & 0xc0) | 0x0f; dp[10] &= 0x3f; 
			dp[3] = (dp[3] & 0xc0) | 0x0f; dp[11] &= 0x3f; 
			dp[4] &= 0xf0;
			break;
		case 2:
			dp[0] &= 0xfc; dp[8] &= 0x3f;
			dp[1] = (dp[1] & 0xf0) | 0x03; dp[ 9] = (dp[ 9] & 0x0f) | 0xc0;
			dp[2] = (dp[2] & 0xf0) | 0x03; dp[10] = (dp[10] & 0x0f) | 0xc0;
			dp[3] = (dp[3] & 0xf0) | 0x03; dp[11] = (dp[11] & 0x0f) | 0xc0;
			dp[4] &= 0xfc; dp[12] &= 0x3f;
			break;
		case 3:
			dp[8] &= 0x0f;
			dp[1] &= 0xfc; dp[ 9] = (dp[ 9] & 0x03) | 0xf0;
			dp[2] &= 0xfc; dp[10] = (dp[10] & 0x03) | 0xf0;
			dp[3] &= 0xfc; dp[11] = (dp[11] & 0x03) | 0xf0;
			dp[12] &= 0x0f;
			break;
	}

	cp[0] = c;
	if (x & 3)
		cp[1] = c;
}


void shot_clear(char x, char y)
{
	plant_draw_field(x >> 2, y);
	if (x & 3)
		plant_draw_field((x >> 2) + 1, y);
}

void shots_add(char x, char y, ShotType type)
{
	if (shots_free != 0xff)
	{
		char	s = shots_free;
		shots_free = shots[s].next;
		shots[s].x = x;
		shots[s].y = y;
		shots[s].type = type;
		shots[s].next = shots_first;
		shots_first = s;
	}
}

const char shot_colors[2] = {VCOL_YELLOW, VCOL_LT_BLUE};

void shots_advance(char step)
{
	char	p = 0xff;
	char	s = shots_first;
	while (s != 0xff)
	{
		shot_clear(shots[s].x, shots[s].y);
		char n = shots[s].next;
		shots[s].x += step;
		char y = shots[s].y >> 2;
		char x = shots[s].x + 20;

		bool	keep = true;

		if (shots[s].x > 144)
			keep = false;
		else if (zombies_left[y] <= x)
		{
			char z = zombies_first[y];
			while (z != 0xff)
			{
				if (zombies[z].x <= x && zombies[z].x > x - 10 && zombies[z].live > 0)
				{
					if (shots[s].type == ST_FROST)
						zombies[z].frozen = 3;
					zombies[z].live -= 2;
					keep = false;
					break;
				}
				z = zombies[z].next;
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

void plants_iterate(char y)
{
	char ps = 0xff;
	char s = plant_first[y];
	char right = zombies_right[y] >> 4;

	while (s != 0xff)
	{
		__assume(s < 10);

		Plant	*	p = plant_grid[y] + s;
		char n = p->next;

		if (p->cool)
			p->cool--;
		else
		{
			switch (p->type)
			{
				case PT_PEASHOOTER_0:
				case PT_PEASHOOTER_1:
					if (s < right)
					{
						p->cool = 8;
						shots_add(16 * s + 12, 4 * y + 1, ST_PEA);
					}
					break;

				case PT_REPEATER_0:
				case PT_REPEATER_1:
					if (s < right)
					{
						p->cool = 8;
						shots_add(16 * s + 16, 4 * y + 1, ST_PEA);
						shots_add(16 * s + 8, 4 * y + 1, ST_PEA);
					}
					break;

				case PT_SNOWPEA_0:
				case PT_SNOWPEA_1:
					if (s < right)
					{
						p->cool = 8;
						shots_add(16 * s + 12, 4 * y + 1, ST_FROST);
					}
					break;

				case PT_POTATOMINE_HIDDEN:
					p->cool = 4;
					p->type = PT_POTATOMINE_0;
					plant_draw(s, y);
					break;

				case PT_EXPLOSION_3:
				case PT_POTATOMINE_EXPLODED:
					p->type = PT_NONE_DAY;
					plant_draw(s, y);

					if (ps != 0xff)
						plant_grid[y][ps].next = n;
					else
						plant_first[y] = n;
					s = ps;
					break;

				case PT_POTATOMINE_0:
				case PT_POTATOMINE_1:
					p->cool = 4;

					char z = zombies_first[y];
					char x = s * 16;
					while (z != 0xff)
					{
						if (zombies[z].x <= x + 32 && zombies[z].x + 16 > x && zombies[z].live > 0)
						{
							zombies[z].live -= 80;
							p->type = PT_POTATOMINE_EXPLODED;
							p->cool = 8;
						}
						z = zombies[z].next;
					}
					if (p->type == PT_POTATOMINE_EXPLODED)
					{
						plant_draw(s, y);			
					}
					break;

				case PT_SUNFLOWER_0:
				case PT_SUNFLOWER_1:
					if (!sun_active)
					{
						p->cool = 64 + (rand() & 63);
						sun_add(s, 50 + 8 * 5 + 32 * y, 0, 25);					
					}
					break;

				case PT_CHOMPER_EAT_0:
				case PT_CHOMPER_EAT_1:
					p->type = PT_CHOMPER_0;
					plant_draw(s, y);
					break;

				case PT_CHERRYBOMB:
					zombies_splash(s * 16 + 28, y, 16, 80);
					if (y > 0)
						zombies_splash(s * 16 + 28, y - 1, 16, 80);
					if (y < 4)
						zombies_splash(s * 16 + 28, y + 1, 16, 80);
					p->type = PT_EXPLOSION_0;
					plant_draw(s, y);
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
};

void plants_animate(void)
{
	for(char i=0; i<4; i++)
	{
		unsigned	r = rand();
		char	x = r & 15, y = (r >> 8) & 7;
		if (x < 9 && y < 5)
		{
			Plant	*	p = plant_grid[y] + x;
			PlantType	t = plant_anim_tab[p->type];
			if (t)
			{
				p->type = t;
				plant_draw(x, y);
				return;
			}
		}
	}
}
