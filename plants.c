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


Plant	plant_grid[5][10];
char	plant_first[5];

void plant_grid_clear(void)
{
	for(char y=0; y<5; y++)
	{
		for(char x=0; x<10; x++)
			plant_grid[y][x].type = PT_NONE;
		plant_first[y] = 0xff;
	}
}

void plant_place(char x, char y, PlantType p)
{
	plant_grid[y][x].type = p;
	plant_grid[y][x].cool = 5 * x + 4 * y + 6;

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

	plant_grid[y][x].next = i;
}

void plant_remove(char x, char y)
{
	plant_grid[y][x].type = PT_NONE;

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
			hdp[j] = PlantsColor1Data[16 * t + 4 * i + j];
		}
		cdp += 40;
		hdp += 40;
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

	char * cdp = Color + 4 * 40 + 4 * x;
	hdp = Screen + 4 * 40 + 4 * x;
	for(char j=0; j<4; j++)
	{
		hdp[j] = VCOL_LT_GREY | (VCOL_DARK_GREY << 4);
	}
}

void plant_draw(char x, char y)
{
	PlantType	p = plant_grid[y][x].type;

	char * hdp = Hires + 5 * 320 + 32 * x + 320 * 4 * y;
	const char * sdp = PlantsHiresData + 8 * 16 * p;

	for(char i=0; i<4; i++)
	{
		for(char j=0; j<8*4; j++)
			hdp[j] = sdp[j];
		hdp += 320;
		sdp += 8 * 4;
	}

	char * cdp = Color + 5 * 40 + 4 * x + 40 * 4 * y;
	hdp = Screen + 5 * 40 + 4 * x + 40 * 4 * y;

	for(char i=0; i<4; i++)
	{
		#pragma unroll(full)
		for(char j=0; j<4; j++)
		{
			cdp[j] = PlantsColor0Data[16 * p + 4 * i + j];
			hdp[j] = PlantsColor1Data[16 * p + 4 * i + j];
		}
		cdp += 40;
		hdp += 40;
	}
}


void plant_draw_field(char x, char y)
{
	char * dp = Hires + 320 * (y + 5) + x * 8;
	char * cp = Color + 40 * (y + 5) + x;
	PlantType p = plant_grid[y >> 2][x >> 2].type;

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

void shot_draw(char x, char y)
{
	__assume(y < 20);

	char * dp = Hires + 320 * (y + 5) + (x & 0xfc) * 2 + 2;
	char * cp = Color + 40 * (y + 5) + (x >> 2);

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

	cp[0] = VCOL_YELLOW;
	if (x & 3)
		cp[1] = VCOL_YELLOW;
}


void shot_clear(char x, char y)
{
	plant_draw_field(x >> 2, y);
	if (x & 3)
		plant_draw_field((x >> 2) + 1, y);
}

void shots_add(char x, char y)
{
	if (shots_free != 0xff)
	{
		char	s = shots_free;
		shots_free = shots[s].next;
		shots[s].x = x;
		shots[s].y = y;
		shots[s].next = shots_first;
		shots_first = s;
	}
}

void shots_advance(void)
{
	char	p = 0xff;
	char	s = shots_first;
	while (s != 0xff)
	{
		shot_clear(shots[s].x, shots[s].y);
		char n = shots[s].next;
		shots[s].x++;
		bool	keep = true;

		if (shots[s].x > 152)
			keep = false;
		else
		{
			char z = zombies_first[shots[s].y >> 2];
			char x = shots[s].x + 12;
			while (z != 0xff)
			{
				if (zombies[z].x <= x && zombies[z].x > x - 10 && zombies[z].live > 0)
				{
					zombies[z].live--;
					keep = false;
					break;
				}
				z = zombies[z].next;
			}
		}

		if (keep)
		{
			shot_draw(shots[s].x, shots[s].y);
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
	char s = plant_first[y];
	while (s != 0xff)
	{
		Plant	*	p = plant_grid[y] + s;
		if (p->cool)
			p->cool--;
		else
		{
			if (p->type == PT_PEASHOOTER)
			{
				p->cool = 32;
				shots_add(16 * s + 12, 4 * y + 1);
			}
		}
		s = p->next;
	}
}
