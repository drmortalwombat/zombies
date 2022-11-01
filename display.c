#include "display.h"
#include "gamemusic.h"


const char SpriteData[] = {
	#embed spd_sprites lzo "zombies.spd"
};

__interrupt void music_irq(void)
{
//	vic.color_border++;
	music_play();
//	vic.color_border--;
}


void display_init(void)
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

	music_init(TUNE_GAME_6);

	rirq_init_kernal();

	for(int i=0; i<5; i++)
	{
		bool	music = (i == 0) || (i == 3);

		zombieMux[i] = rirq_alloc(ZOMBIE_SPRITES * 4 + 1 + music);

		for(int j=0; j<ZOMBIE_SPRITES; j++)
		{
			rirq_write(zombieMux[i], 0 * ZOMBIE_SPRITES + j, &(vic.spr_pos[j].y), 50 + 5 * 8 + 4 * 8 * i + 8);
			rirq_write(zombieMux[i], 1 * ZOMBIE_SPRITES + j, &(vic.spr_pos[j].x), 0);
			rirq_write(zombieMux[i], 2 * ZOMBIE_SPRITES + j, Screen + 0x3f8 + j, 16);
			rirq_write(zombieMux[i], 3 * ZOMBIE_SPRITES + j, &(vic.spr_color[j]), VCOL_MED_GREY);
		}
		rirq_write(zombieMux[i], 4 * ZOMBIE_SPRITES, &vic.spr_msbx, 0);
		if (music)
			rirq_call(zombieMux[i], 4 * ZOMBIE_SPRITES + 1, music_irq);

		rirq_set(i, 50 + 5 * 8 + 4 * 8 * i, zombieMux[i]);
	}

	rirq_build(&menuMux, 4);
	rirq_write(&menuMux, 0, &(vic.spr_pos[6].y), 50 - 10);
	rirq_write(&menuMux, 1, &(vic.spr_pos[6].x), 24);
	rirq_write(&menuMux, 2, &vic.spr_msbx, 0);
	rirq_write(&menuMux, 3, Screen + 0x3f8 + 6, 16 + 109);
	rirq_set(6, 10, &menuMux);

	rirq_build(&cursorMux, 4);
	rirq_write(&cursorMux, 0, &(vic.spr_pos[6].y), 50 - 10);
	rirq_write(&cursorMux, 1, &(vic.spr_pos[6].x), 24);
	rirq_write(&cursorMux, 2, &vic.spr_msbx, 0);
	rirq_write(&cursorMux, 3, Screen + 0x3f8 + 6, 16 + 108);
	rirq_set(7, 50 + 32, &cursorMux);

	rirq_sort();

	rirq_start();

	vic.spr_enable = 0x3f;
	vic.spr_multi = 0x3f;
}
