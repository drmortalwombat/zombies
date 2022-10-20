#include "zombies.h"
#include "display.h"


void zombies_init(void)
{
	for(char i=0; i<5; i++)
		zombies_first[i] = 0xff;

	for(char i=0; i<31; i++)
		zombies[i].next = i + 1;

	zombies[31].next = 0xff;
	zombies_free = 0;
}

void zombies_add(char x, char y, ZombieType type)
{
	if (zombies_free != 0xff)
	{
		char	s = zombies_free;
		zombies_free = zombies[s].next;
		zombies[s].x = x;
		zombies[s].y = y;
		zombies[s].phase = 0;
		zombies[s].live = 1;
		zombies[s].type = type;

		zombies[s].next = zombies_first[y];
		zombies_first[y] = s;
	}
}

char	zombies_msbx[5];
char	zombies_basemsbx;

void zombies_set_msbx(char mask, char val)
{
	if ((zombies_basemsbx & mask) != val)
	{
		zombies_basemsbx = (zombies_basemsbx & ~mask) | val;
		for(char y=0; y<5; y++)
		{
			rirq_data(zombieMux[y], 3 * ZOMBIE_SPRITES, zombies_msbx[y] | zombies_basemsbx);
		}
	}
}

void zombies_advance(char y)
{
	char msbx = 0;
	char nz = 0;

	char	p = 0xff;
	char s = zombies_first[y];
	while (s != 0xff)
	{
		char n = zombies[s].next;

		zombies[s].x --;
		if (zombies[s].live == 0)
		{
			switch (zombies[s].type)
			{
				case ZT_BASE:
					break;
				case ZT_HEAD:
					zombies[s].type = ZT_BASE;
					zombies[s].live = 1;
					break;
			}
		}

		if (zombies[s].live > 0 && zombies[s].x > 0)
		{
			zombies[s].phase++;
			if (zombies[s].phase == 6)
				zombies[s].phase = 0;

			unsigned	x = zombies[s].x << 1;
			char		img = zombies[s].phase + 16 + 16 * zombies[s].type;

			rirq_data(zombieMux[y], 1 * ZOMBIE_SPRITES + nz, x & 0xff);
			rirq_data(zombieMux[y], 2 * ZOMBIE_SPRITES + nz, img);

			if (x & 0x100)
				msbx |= 1 << nz;
			nz++;

			p = s;
		}
		else
		{
			if (p == 0xff)
				zombies_first[y] = n;
			else
				zombies[p].next = n;
			zombies[s].next = zombies_free;
			zombies_free = s;

		}

		s = n;
	}

	while (nz < ZOMBIE_SPRITES)
	{
		rirq_data(zombieMux[y], 1 * ZOMBIE_SPRITES + nz, 0);
		nz++;
	}

	zombies_msbx[y] = msbx;
	rirq_data(zombieMux[y], 3 * ZOMBIE_SPRITES, msbx | zombies_basemsbx);
}
