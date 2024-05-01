#include "display.h"
#include "gamemusic.h"
#include <audio/sidfx.h>

#pragma section( sprites, 0)

#pragma region( sprites, 0xbe00, 0xd000, , , {sprites} )

#pragma data(sprites)

const char SpriteData[] = {
	#embed spd_sprites lzo "zombies.spd"
};

#pragma data(data)

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

const char FontData[] = {
	#embed ctm_chars "chars.ctm"
};

const char DigitsHiresData[] = {
	#embed ctm_chars "digits.ctm"
};

__striped char * const HiresTab[25] = {
	Hires +  0 * 320, Hires +  1 * 320, Hires +  2 * 320, Hires +  3 * 320,
	Hires +  4 * 320, Hires +  5 * 320, Hires +  6 * 320, Hires +  7 * 320,
	Hires +  8 * 320, Hires +  9 * 320, Hires + 10 * 320, Hires + 11 * 320,
	Hires + 12 * 320, Hires + 13 * 320, Hires + 14 * 320, Hires + 15 * 320,
	Hires + 16 * 320, Hires + 17 * 320, Hires + 18 * 320, Hires + 19 * 320,
	Hires + 20 * 320, Hires + 21 * 320, Hires + 22 * 320, Hires + 23 * 320,
	Hires + 24 * 320
};

__striped char * const ColorTab[25] = {
	Color +  0 * 40, Color +  1 * 40, Color +  2 * 40, Color +  3 * 40,
	Color +  4 * 40, Color +  5 * 40, Color +  6 * 40, Color +  7 * 40,
	Color +  8 * 40, Color +  9 * 40, Color + 10 * 40, Color + 11 * 40,
	Color + 12 * 40, Color + 13 * 40, Color + 14 * 40, Color + 15 * 40,
	Color + 16 * 40, Color + 17 * 40, Color + 18 * 40, Color + 19 * 40,
	Color + 20 * 40, Color + 21 * 40, Color + 22 * 40, Color + 23 * 40,
	Color + 24 * 40
};

__striped char * const ScreenTab[25] = {
	Screen +  0 * 40, Screen +  1 * 40, Screen +  2 * 40, Screen +  3 * 40,
	Screen +  4 * 40, Screen +  5 * 40, Screen +  6 * 40, Screen +  7 * 40,
	Screen +  8 * 40, Screen +  9 * 40, Screen + 10 * 40, Screen + 11 * 40,
	Screen + 12 * 40, Screen + 13 * 40, Screen + 14 * 40, Screen + 15 * 40,
	Screen + 16 * 40, Screen + 17 * 40, Screen + 18 * 40, Screen + 19 * 40,
	Screen + 20 * 40, Screen + 21 * 40, Screen + 22 * 40, Screen + 23 * 40,
	Screen + 24 * 40
};

__zeropage	char		back_color;

// Interrupt for music
__interrupt void music_irq(void)
{
//	vic.color_border++;
	music_play();
//	vic.color_border--;
}

// Interrupt for sound effects
__interrupt void sidfx_irq(void)
{
	sidfx_loop_2();
}

void display_init(void)
{
	mmap_set(MMAP_NO_BASIC);

	// Install trampoline
	mmap_trampoline();

	// Disable interrupts while setting up
	__asm { sei };

	// Kill CIA interrupts
	cia_init();

	// All RAM
	mmap_set(MMAP_RAM);

	// Copy sprite data into hires to avoid overlap when
	// decompressing, then decompress
	memcpy(Hires, SpriteData, sizeof(SpriteData));
	oscar_expand_lzo(Sprites, Hires);

	// RAM and IO
	mmap_set(MMAP_NO_ROM);

	// Init screen pointers
	vic_setmode(VICM_HIRES_MC, Screen, Hires);

	vic.color_border = VCOL_BLACK;
	vic.color_back = VCOL_BLACK;

	spr_init(Screen);
	vic.spr_mcolor0 = VCOL_BLACK;
	vic.spr_mcolor1 = VCOL_WHITE;

	// Clear screen
	memset(Screen, 0, 1000);
	memset(Color, 0, 1000);

	music_init(TUNE_GAME_6);
	sidfx_init();

	// Init raster IRQ system for no ROM memory map
	rirq_init_io();

	// Initialize the five row IRQs for the zombies
	for(int i=0; i<5; i++)
	{
		// Music IRQ in first and fourth row
		bool	music = (i == 0) || (i == 3);

		RIRQCode	*	zm = rirq_alloc(ZOMBIE_SPRITES * 4 + 1 + music);

		// Init zombie sprite multiplexer for the row
		zombieMux[i] = zm;
		for(int j=0; j<ZOMBIE_SPRITES; j++)
		{
			rirq_write(zm, 0 * ZOMBIE_SPRITES + j, &(vic.spr_pos[j].y), 50 + 5 * 8 + 4 * 8 * i + 8);
			rirq_write(zm, 1 * ZOMBIE_SPRITES + j, &(vic.spr_pos[j].x), 0);
			rirq_write(zm, 2 * ZOMBIE_SPRITES + j, Screen + 0x3f8 + j, 16);
			rirq_write(zm, 3 * ZOMBIE_SPRITES + j, &(vic.spr_color[j]), VCOL_MED_GREY);
		}
		rirq_write(zm, 4 * ZOMBIE_SPRITES, &vic.spr_msbx, 0);

		// Add music IRQ if required
		if (music)
			rirq_call(zm, 4 * ZOMBIE_SPRITES + 1, music_irq);

		// Place raster IRQ
		rirq_set(i, 50 + 5 * 8 + 4 * 8 * i, zm);
	}

	// IRQ for menu and sound effects on top of screen
	rirq_build(&menuMux, 5);
	rirq_write(&menuMux, 0, &(vic.spr_pos[6].y), 50 - 10);
	rirq_write(&menuMux, 1, &(vic.spr_pos[6].x), 24);
	rirq_write(&menuMux, 2, &vic.spr_msbx, 0);
	rirq_write(&menuMux, 3, Screen + 0x3f8 + 6, 16 + 109);
	rirq_call(&menuMux, 4, sidfx_irq);
	rirq_set(6, 10, &menuMux);

	// IRQ for cursor sprite at bottom of menu
	rirq_build(&cursorMux, 4);
	rirq_write(&cursorMux, 0, &(vic.spr_pos[6].y), 50 - 10);
	rirq_write(&cursorMux, 1, &(vic.spr_pos[6].x), 24);
	rirq_write(&cursorMux, 2, &vic.spr_msbx, 0);
	rirq_write(&cursorMux, 3, Screen + 0x3f8 + 6, 16 + 108);
	rirq_set(7, 50 + 32, &cursorMux);

	// Start IRQs
	rirq_sort();

	rirq_start();

	vic.spr_enable = 0x3f;
	vic.spr_multi = 0x3f;
}

void disp_color_price(char dx, char dy)
{
	// Draw price in grey
	char * cdp = ScreenTab[dy] + dx;
	for(char i=0; i<4; i++)
		cdp[i] = VCOL_LT_GREY | (VCOL_DARK_GREY << 4);
}

void disp_put_price(unsigned v, char dx, char dy)
{
	char	c[4], c0 = 10;

	// Convert price to digits
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

	char * hdp = HiresTab[dy] + 8 * dx;

	// Draw digits into hires bitmap
	for(char i=0; i<4; i++)
	{
		const char * sdp = DigitsHiresData + 8 * c[i];

		for(char j=0; j<8; j++)
			hdp[j] = sdp[j];
		hdp += 8;
	}

}

void disp_put_noprice(char dx, char dy)
{
	char * hdp = HiresTab[dy] + 8 * dx;

	const char * sdp = DigitsHiresData + 80;

	// Clear digit hires bitmap
	for(char i=0; i<4; i++)
	{
		for(char j=0; j<8; j++)
			hdp[j] = sdp[j];
		hdp += 8;
	}

}

// Character expand for outline and shadow effect
static const char cexpand[16] = {
	0x00, 0x03, 0x0c, 0x0f, 0x30, 0x33, 0x3c, 0x3f,
	0xc0, 0xc3, 0xcc, 0xcf, 0xf0, 0xf3, 0xfc, 0xff
};

// Draw character with outline and shadow
void text_pline(char * hp, char m, char l)
{
	char t0 = hp[0];
	char t1 = hp[8];
	char t2 = hp[16];

	char m0 = m | (m >> 1) | (m >> 2);
	char m1 = (m << 3) | (m << 2) | (m << 1);

	t0 &= ~cexpand[m0 >> 4];
	t1 &= ~cexpand[m0 & 0x0f];
	t2 &= ~cexpand[m1 & 0x0f];

	l >>= 1;

	t0 |= cexpand[l >> 4];
	t1 |= cexpand[l & 0x0f];

	hp[ 0] = t0;
	hp[ 8] = t1;
	hp[16] = t2;
}

void text_put(char x, char y, char color, const char * t)
{
	// Target position
	char	*	hp = HiresTab[y] + 8 * x;
	char	*	cp = ColorTab[y] + x;
	char		ci = 0;

	// Loop over characters in string
	while (char c = t[ci])
	{
		// Line break
		if (c == '\n')
		{
			y += 2;
			hp = HiresTab[y] + 8 * x;
			cp = ColorTab[y] + x;
		}
		else
		{
			// Get font data
			const char * fp = FontData + 8 * (c & 0x3f);
			cp[0] = color;
			cp[1] = color;

			// Expand first pixel outline
			text_pline(hp, fp[0], 0);

			// Expand main character lines
			char pl = 0;
			for(char i=0; i<7; i++)
			{
				char l = fp[i];
				char n = fp[i + 1];

				text_pline(hp + i + 1, (pl | l | n) | ((pl | l) >> 1), l);
				pl = l;
			}

			// Expand bottom lines and shadow
			text_pline(hp + 320, fp[6] | (fp[6] >> 1), 0);
			text_pline(hp + 321, fp[6] >> 1, 0);

			// Next character
			hp += 16;
			cp += 2;
		}

		ci++;
	}
}

void text_put_2(char x, char y, char color1, char color2, const char * t)
{
	char	*	hp = HiresTab[y] + 8 * x;
	char	*	cp = ColorTab[y] + x;
	char		ci = 0;

	while (char c = t[ci])
	{
		if (c == '\n')
		{
			y += 2;
			hp = HiresTab[y] + 8 * x;
			cp = ColorTab[y] + x;
		}
		else
		{
			const char * fp = FontData + 8 * (c & 0x3f);
			cp[0] = color1;
			cp[1] = color1;
			cp[40] = color2;
			cp[41] = color2;

			text_pline(hp, fp[0], 0);

			char pl = 0;
			for(char i=0; i<7; i++)
			{
				char l = fp[i];
				char n = fp[i + 1];

				text_pline(hp + 2 * i + 1, (pl | l) | ((pl | l) >> 1), l);
				if (i == 3)
					hp += 312;
				text_pline(hp + 2 * i + 2, (l | n) | (l >> 1), l);
				pl = l;
			}

			text_pline(hp + 15, fp[6] | (fp[6] >> 1), 0);
			text_pline(hp + 328, fp[6] >> 1, 0);

			hp -= 312;
			hp += 16;
			cp += 2;
		}

		ci++;
	}
}

// Map to day and night background color
static inline char plant_color_map(char c)
{
	return ((c & 0xf0) == (VCOL_PURPLE << 4)) ? (c & 0x0f) | back_color : c;
}

void disp_put_char(PlantType type, char sx, char sy, char dx, char dy)
{
	__assume(dx < 40);
	__assume(dy < 25);
	__assume(sx < 4);
	__assume(sy < 4);

	// Display position
	char * hdp = HiresTab[dy] + 8 * dx;
	const char * sdp = PlantsHiresData + 8 * 16 * type + 8 * sx + 32 * sy;

	// Bitmap data
	for(char j=0; j<8; j++)
		hdp[j] = sdp[j];

	// Color data
	char * cdp = ColorTab[dy];
	hdp = ScreenTab[dy];

	const char * scdp = PlantsColor0Data + 16 * type + 4 * sy;
	const char * shdp = PlantsColor1Data + 16 * type + 4 * sy;

	cdp[dx] = scdp[sx];
	hdp[dx] = plant_color_map(shdp[sx]);
}

void disp_put_tile(PlantType type, char dx, char dy)
{
	__assume(dx < 36);
	__assume(dy < 21);

	// Display position
	char * hdp = HiresTab[dy] + 8 * dx;
	const char * sdp = PlantsHiresData + 8 * 16 * type;

	// Bitmap data
	for(char i=0; i<4; i++)
	{
		for(signed char j=31; j>=0; j--)
			hdp[j] = sdp[j];
		hdp += 320;
		sdp += 32;
	}

	char * cdp = ColorTab[dy] + dx;
	hdp = ScreenTab[dy] + dx;

	// Color data
	const char * scdp = PlantsColor0Data + 16 * type;
	const char * shdp = PlantsColor1Data + 16 * type;

	for(char i=0; i<4; i++)
	{
		for(char j=0; j<4; j++)
		{
			cdp[j] = scdp[j];
			hdp[j] = plant_color_map(shdp[j]);
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

	char * cdp = ColorTab[dy] + dx;
	char * hdp = ScreenTab[dy] + dx;

	for(char i=0; i<4; i++)
	{
		for(char j=0; j<4; j++)
		{
			cdp[j] = color_grey[PlantsColor0Data[16 * type + 4 * i + j] & 0x0f];
			char c = plant_color_map(PlantsColor1Data[16 * type + 4 * i + j]);
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

	char * cdp = ColorTab[dy] + dx;
	char * hdp = ScreenTab[dy] + dx;

	for(char i=0; i<4; i++)
	{
		for(char j=0; j<4; j++)
		{
			cdp[j] = PlantsColor0Data[16 * type + 4 * i + j];
			char c = plant_color_map(PlantsColor1Data[16 * type + 4 * i + j]);
			hdp[j] = c;
		}
		cdp += 40;
		hdp += 40;
	}	
}
