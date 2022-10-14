#include <c64/vic.h>
#include <c64/sprites.h>
#include <c64/rasterirq.h>
#include <c64/memmap.h>
#include <oscar.h>
#include <string.h>
#include <stdlib.h>
#include "gamemusic.h"

const char SpriteData[] = {
	#embed spd_sprites lzo "zombies.spd"
};

const char PlantsHiresData[] = {
	#embed ctm_chars "plants.ctm"
};

const char PlantsColor0Data[] = {
	#embed ctm_attr1 "plants.ctm"
};

const char PlantsColor1Data[] = {
	#embed ctm_attr2 "plants.ctm"
};


static char * const Screen	=	(char *)0xc800;
static char * const Screen2=	(char *)0xcc00;
static char * const Color	=	(char *)0xd800; 
static char * const Sprites =  (char *)0xd000;
static char * const Hires	=	(char *)0xe000;

enum PlantType
{
	PT_NONE,
	PT_PEASHOOTER,
	PT_SUNFLOWER,
	PT_WALLNUT,
	PT_CHERRYBOMB,
	PT_SUNSHROOM,
	PT_CACTUS,
	PT_POTATOMINE,
	PT_REPEATER,

	NUM_PLANT_TYPES
};

struct Plant
{	
	char		next;
	PlantType	type;
}	plants[5][10];


void plant_expand(char x, char y, char p)
{
	plants[y][x] = p;

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
		for(char j=0; j<4; j++)
		{
			cdp[j] = PlantsColor0Data[16 * p + 4 * i + j];
			hdp[j] = PlantsColor1Data[16 * p + 4 * i + j];
		}
		cdp += 40;
		hdp += 40;
	}
}

RIRQCode	*	zombieMux[6];
RIRQCode		musicMux;
char			zombieMsbX[6];

#define ZOMBIE_SPRITES	6

void zombie_move(char row, char zi, unsigned x, char phase)
{
	rirq_data(zombieMux[row], 1 * ZOMBIE_SPRITES + zi, x & 0xff);
	rirq_data(zombieMux[row], 2 * ZOMBIE_SPRITES + zi, phase + 64);

	if (x & 0x100)
		zombieMsbX[row] |= 1 << zi;
	else
		zombieMsbX[row] &= ~(1 << zi);

	rirq_data(zombieMux[row], 3 * ZOMBIE_SPRITES, zombieMsbX[row]);
}

__interrupt void music_irq(void)
{
	vic.color_border++;
	music_play();
	vic.color_border--;
}

char shot_mask[8] = {
	0x3c, 0xff, 0xff, 0xff, 0xff, 0x3c, 0x00, 0x00
};
char shot_color[8] = {
	0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00, 0x00
};



void shot_draw(char x, char y)
{
	char * dp = Hires + 320 * (y + 5) + (x & 0xfc) * 2;
	char * cp = Color + 40 * (y + 5) + (x >> 2);

	char sx = 8 - (x & 3) * 2;

	for(char i=0; i<8; i++)
	{
		unsigned	m = ~(shot_mask[i] << sx);
		unsigned	d = shot_color[i] << sx;

		dp[i] = (dp[i] & (m >> 8)) | (d >> 8);
		dp[i + 8] = (dp[i + 8] & m) | d;
	}

	cp[0] = VCOL_YELLOW;
	if (x & 3)
		cp[1] = VCOL_YELLOW;
}

void field_clear(char x, char y)
{
	char * dp = Hires + 320 * (y + 5) + x * 8;
	char * cp = Color + 40 * (y + 5) + x;
	char p = plant_grid[y >> 2][x >> 2];

	const char * sp = PlantsHiresData + 8 * 16 * p + 32 * (y & 3) + 8 * (x & 3);

	for(char i=0; i<8; i++)
		dp[i] = sp[i];
	cp[0] = PlantsColor0Data[16 * p + 4 * (y & 3) + (x & 3)];
}

void shot_clear(char x, char y)
{
	field_clear(x >> 2, y);
	if (x & 3)
		field_clear((x >> 2) + 1, y);
}

int main(void)
{
	mmap_set(MMAP_NO_BASIC);

	mmap_trampoline();

	mmap_set(MMAP_RAM);

	oscar_expand_lzo(Sprites, SpriteData);

	mmap_set(MMAP_NO_ROM);

	vic_setmode(VICM_HIRES_MC, Screen, Hires);

	vic.color_border = VCOL_BLACK;
	vic.color_back = VCOL_BLACK;

	spr_init(Screen);
	vic.spr_mcolor0 = VCOL_BLACK;
	vic.spr_mcolor1 = VCOL_WHITE;

	memset(Screen, 0, 1000);
	memset(Color, 0, 1000);


	for(char y=0; y<5; y++)
	{
		for(char x=0; x<10; x++)
		{
			char r = rand() & 31;
			plant_expand(x, y, r < 8 ? r + 1 : 0);
		}
	}

	for(char i=0; i<20; i++)
	{
		shot_draw(5 * i, i);
	}

	for(char i=0; i<20; i++)
	{
		shot_clear(5 * i, i);
	}

	music_init(1);

	rirq_init_kernal();

	for(int i=0; i<6; i++)
	{
		bool	music = (i == 0) || (i == 3);

		zombieMux[i] = rirq_alloc(ZOMBIE_SPRITES * 3 + 1 + music);

		for(int j=0; j<ZOMBIE_SPRITES; j++)
		{
			rirq_write(zombieMux[i], 0 * ZOMBIE_SPRITES + j, &(vic.spr_pos[j].y), 50 + 5 * 8 + 4 * 8 * i + 8);
			rirq_write(zombieMux[i], 1 * ZOMBIE_SPRITES + j, &(vic.spr_pos[j].x), 24 + 40 * j);
			rirq_write(zombieMux[i], 2 * ZOMBIE_SPRITES + j, Screen + 0x3f8 + j, 64 + j);
		}
		rirq_write(zombieMux[i], 3 * ZOMBIE_SPRITES, &vic.spr_msbx, 0);
		if (music)
			rirq_call(zombieMux[i], 3 * ZOMBIE_SPRITES + 1, music_irq);

		rirq_set(i, 50 + 5 * 8 + 4 * 8 * i, zombieMux[i]);
	}

	rirq_sort();

	rirq_start();

	for(int j=0; j<ZOMBIE_SPRITES; j++)
		vic.spr_color[j] = VCOL_MED_GREY;
	vic.spr_enable = 0x3f;
	vic.spr_multi = 0x3f;

	int	t = 0;
	for(;;)
	{
		for(int i=0; i<6; i++)
		{
			for(int j=0; j<6; j++)
			{
				zombie_move(i, j, 360 - 3 * i - t - 30 * j, (t + i + j) % 6);
			}
		}

		t++;
		if (t > 300)
			t = 0;

		for(char i=0; i<20; i++)
		{
			shot_clear(t + 5 * i, i);
			shot_draw(t + 5 * i + 1, i);
		}

//		vic_waitFrame();
//		vic_waitFrame();

//		vic_waitFrame();
//		vic_waitFrame();
	}


	return 0;
}
