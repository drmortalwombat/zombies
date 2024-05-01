#include "zombies.h"
#include "display.h"
#include "plants.h"
#include <audio/sidfx.h>

SIDFX	SIDFXZombieFume[1] = {{
	6000, 4096,
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_240,
	-400, 0,
	2, 12,
	10
}};

SIDFX	SIDFXZombieChomp[1] = {{
	2000, 4096,
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_240,
	-100, 0,
	2, 12,
	0
}};

// MSBs for the zombie sprites in all rows
char	zombies_msbx[5];
char	zombies_basemsbx;


void zombies_clear(void)
{
	// Move the zombies off screen
	for(char y=0; y<5; y++)
	{
		for(char i=0; i < ZOMBIE_SPRITES; i++)
			rirq_data(zombieMux[y], 1 * ZOMBIE_SPRITES + i, 0);

		zombies_msbx[y] = 0;
		rirq_data(zombieMux[y], 4 * ZOMBIE_SPRITES, zombies_basemsbx);	
	}
}

void zombies_init(void)
{
	// Clear zombie list
	for(char i=0; i<5; i++)
	{
		zombies_first[i] = 0xff;
		zombies_count[i] = 0;
	}

	// Init zombie free list
	for(char i=0; i<31; i++)
		zombies[i].next = i + 1;

	zombies[31].next = 0xff;
	zombies_free = 0;
}

bool zombies_add(char x, char y, ZombieType type, char extra)
{
	// Check for zombie in free list and sprite limit of 6 per row
	if (zombies_free != 0xff && zombies_count[y] < 6)
	{
		// Get a zombie node from the free list
		char	s = zombies_free;
		zombies_free = zombies[s].next;

		// Init zombie node
		zombies[s].x = x;
		zombies[s].phase = 0;
		zombies[s].delay = 0;		
		zombies[s].speed = 112 + (rand() & 15);
		zombies[s].extra = extra;

		// Set zombie live per type
		switch (type)
		{
			case ZOMBIE_BASE:
			case ZOMBIE_BACKUP:
			case ZOMBIE_BACKUP_RAISE:
				zombies[s].live = 20;
				break;
			case ZOMBIE_CONE:
				zombies[s].live = 44;
				break;
			case ZOMBIE_BUCKET:
				zombies[s].live = 117;
				break;			
			case ZOMBIE_POLE:
				zombies[s].live = 50;
				zombies[s].speed *= 2;
				break;
			case ZOMBIE_PAPER:
				zombies[s].live = 15;
				break;			
			case ZOMBIE_SCREENDOOR:
				zombies[s].live = 117;
				break;
			case ZOMBIE_FOOTBALL:
				zombies[s].live = 127;
				zombies[s].speed *= 2;
				break;
			case ZOMBIE_DANCER:
				zombies[s].live = 50;
				break;
			case ZOMBIE_RESURRECT:
				zombies[s].live = 20;
				break;
		}
		zombies[s].frozen = 0;
		zombies[s].type = type;

		// Attach zombie node to row list
		zombies[s].next = zombies_first[y];
		zombies_first[y] = s;
		zombies_count[y]++;

		// Success
		return true;
	}
	else
		return false;
}

void zombies_grave(ZombieType type)
{
	// Loop over all rows
	for(char y=0; y<5; y++)
	{
		// Loop over all plants
		char p = plant_first[y];
		while (p != 0xff)
		{
			// If it is a tombstone, add a zombie
			if (plant_grid[y][p].type == PT_TOMBSTONE)
				zombies_add(p * 16 + 24, y, type, 0);
			p = plant_grid[y][p].next;
		}
	}
}

void zombies_set_msbx(char mask, char val)
{
	// Check if msb changed
	if ((zombies_basemsbx & mask) != val)
	{
		// Loop over all rows and set zombie msb		
		zombies_basemsbx = (zombies_basemsbx & ~mask) | val;
		for(char y=0; y<5; y++)
		{
			rirq_data(zombieMux[y], 4 * ZOMBIE_SPRITES, zombies_msbx[y] | zombies_basemsbx);
		}
	}
}

void zombie_damage(char s, char damage)
{
	// New (un)live of zombie
	int live = zombies[s].live - damage;
	// Clamp
	if (live < -128)
		live = -128;
	zombies[s].live = live;
	
	// Mark zombie for flashing
	zombies[s].frozen |= 0x80;
}

void zombies_splash(char x, char y, char w, char damage)
{
	// Loop over all zombies in row
	char s = zombies_first[y];
	while (s != 0xff)
	{
		// Damage the ones near to us
		if (zombies[s].x < x + w && zombies[s].x + w >= x)
			zombie_damage(s, damage);

		s = zombies[s].next;
	}
}

void zombies_freeze_all(char frost)
{
	// Loop over all rows
	for(char y=0; y<5; y++)
	{
		// Loop over all zombies
		char s = zombies_first[y];
		while (s != 0xff)
		{
			// Freeze
			zombies[s].frozen += frost;
			s = zombies[s].next;
		}		
	}
}

void zombies_fume(char x, char y, char w)
{
	bool	fume = false;

	// Loop over all zombies in row
	char s = zombies_first[y];
	while (s != 0xff)
	{
		// Is zombie in fume zone
		if (zombies[s].x < x + w && zombies[s].x >= x)
		{
			// Add damage
			if (zombies[s].type == ZOMBIE_SCREENDOOR)
				zombie_damage(s, 12);
			else
				zombie_damage(s, 2);
			fume = true;
		}
		s = zombies[s].next;
	}	

	if (fume)
		sidfx_play(2, SIDFXZombieFume, 1);
}

// Find zombie backup dancers
void zombie_find_backup(char y, char s, char * b)
{
	// So far no dancers found
	b[0] = 0xff;	
	b[1] = 0xff;
	b[2] = 0xff;
	b[3] = 0xff;

	// Loop over zombies in same row
	char z = zombies_first[y];
	while (z != 0xff)
	{
		// Is this a backup dancer
		if (zombies[z].type == ZOMBIE_BACKUP && zombies[z].extra == s)
		{
			if (zombies[z].x < zombies[s].x)
				b[1] = z;
			else
				b[2] = z;
		}
		z = zombies[z].next;
	}

	if (y > 0)
	{
		// Loop over zombies in row above
		z = zombies_first[y - 1];
		while (z != 0xff)
		{
			// Is this a backup dancer
			if (zombies[z].type == ZOMBIE_BACKUP && zombies[z].extra == s)
				b[0] = z;
			z = zombies[z].next;
		}		
	}

	if (y < 4)
	{
		// Loop over zombies in row below
		z = zombies_first[y + 1];
		while (z != 0xff)
		{
			// Is this a backup dancer
			if (zombies[z].type == ZOMBIE_BACKUP && zombies[z].extra == s)
				b[3] = z;
			z = zombies[z].next;
		}				
	}
}

// Keep dancer zombie in synch with backup dancers
void zombie_sync_backup(char y, char s)
{
	char	b[4];
	zombie_find_backup(y, s, b);

	char minx = zombies[s].x;
	if (b[0] != 0xff && zombies[b[0]].x > minx + 1)
		minx = zombies[b[0]].x;
	if (b[1] != 0xff && zombies[b[1]].x + 16 > minx + 1)
		minx = zombies[b[1]].x + 16;
	if (b[2] != 0xff && zombies[b[2]].x - 16 > minx + 1)
		minx = zombies[b[2]].x - 16;
	if (b[3] != 0xff && zombies[b[3]].x > minx + 1)
		minx = zombies[b[3]].x;
	zombies[s].x = minx;
}

// Release backup dancers if main zombie undies
void zombie_free_backup(char y, char s)
{
	char	b[4];
	zombie_find_backup(y, s, b);
	if (b[0] != 0xff)
		zombies[b[0]].type = ZOMBIE_BASE;
	if (b[1] != 0xff)
		zombies[b[1]].type = ZOMBIE_BASE;
	if (b[2] != 0xff)
		zombies[b[2]].type = ZOMBIE_BASE;
	if (b[3] != 0xff)
		zombies[b[3]].type = ZOMBIE_BASE;
}

// Add missing backup dancers to zombie
void zombie_add_backup(char y, char s)
{
	char	b[4];
	zombie_find_backup(y, s, b);
	if (y > 0 && b[0] == 0xff)
		zombies_add(zombies[s].x, y - 1, ZOMBIE_BACKUP_RAISE, s);
	if (b[1] == 0xff)
		zombies_add(zombies[s].x - 16, y, ZOMBIE_BACKUP_RAISE, s);
	if (b[2] == 0xff)
		zombies_add(zombies[s].x + 16, y, ZOMBIE_BACKUP_RAISE, s);
	if (y < 4 && b[3] == 0xff)
		zombies_add(zombies[s].x, y + 1, ZOMBIE_BACKUP_RAISE, s);
}

bool zombies_advance(char y)
{
	// No MSB so far
	char msbx = 0;
	char nz = 0;

	// Left and right boundary
	char	left = 0xff, right = 0;
	bool	lost = false;

	// Loop over all zombies in row
	char	p = 0xff;
	char s = zombies_first[y];

	while (s != 0xff)
	{
		char n = zombies[s].next;

		char color = VCOL_MED_GREY;
		char px = (zombies[s].x - 16) >> 4;

		// Check if zombie is realy "dead"
		if (zombies[s].live <= 0)
		{
			switch (zombies[s].type)
			{
				case ZOMBIE_POLE:
				case ZOMBIE_VAULT:
				case ZOMBIE_BASE:
				case ZOMBIE_ANGRY:
				case ZOMBIE_BACKUP:
					// Base zombies become a corps
					zombies[s].type = ZOMBIE_CORPSE;
					zombies[s].phase = 0;
					break;
				case ZOMBIE_CONE:
				case ZOMBIE_BUCKET:
				case ZOMBIE_SCREENDOOR:
				case ZOMBIE_FOOTBALL:
					// Protected zombies become base zombies and get a second live
					zombies[s].type = ZOMBIE_BASE;
					zombies[s].live += 20;
					break;
				case ZOMBIE_PAPER:
					zombies[s].type = ZOMBIE_ANGRY;
					zombies[s].live += 27;
					zombies[s].speed *= 2;
					break;					
				case ZOMBIE_CORPSE:
					// Corpse zombies decay
					zombies[s].phase++;
					if (zombies[s].phase == 4)
						zombies[s].type = ZOMBIE_NONE;
					break;
				case ZOMBIE_DANCER:
					// Backups of dancer are released
					zombie_free_backup(y, s);
					zombies[s].type = ZOMBIE_CORPSE;
					zombies[s].phase = 0;
					break;
			}
		}

		if (zombies[s].type >= ZOMBIE_BASE)
		{
			// Flash zombie
			if (zombies[s].frozen & 0x80)
			{
				zombies[s].frozen &= 0x7f;
				color = VCOL_LT_GREY;
			}

			// Turn zombie blue
			if (zombies[s].frozen)
			{
				color = VCOL_LT_BLUE;
				zombies[s].frozen--;
			}
			else if (zombies[s].type == ZOMBIE_VAULT)
			{
				// Vaulting zombie

				unsigned	d = zombies[s].delay + 128;
				zombies[s].delay = d;
				zombies[s].x -= 2;
				
				if (d & 0x0100)
				{
					zombies[s].phase++;
					if (zombies[s].phase >= 4)
					{
						zombies[s].type = ZOMBIE_BASE;						
					}
				}
			}
			else if (zombies[s].type != ZOMBIE_BACKUP_RAISE && zombies[s].type != ZOMBIE_RESURRECT && px < 9 && plant_grid[y][px].type > PT_GROUND)
			{
				// Position blocked
				if (zombies[s].type == ZOMBIE_POLE)
				{
					zombies[s].type = ZOMBIE_VAULT;
					zombies[s].phase = 0;
					zombies[s].delay = 0;
				}
				else
				{
					zombies[s].phase++;
					// Eat from plant every 4th cycle
					if (zombies[s].phase >= 10)
					{
						zombies[s].phase = 6;
						// Reduce plant live
						plant_grid[y][px].live--;
						if (plant_grid[y][px].live == 0)
						{
							// Plant killed (may need to restore grave if it was a grave digger)
							if (plant_grid[y][px].type == PT_GRAVEDIGGER_0)
								plant_grid[y][px].type = PT_TOMBSTONE;
							else
								plant_remove(px, y);
							plant_draw(px, y);
						}

						sidfx_play(2, SIDFXZombieChomp, 1);
					}
				}
			}
			else
			{
				unsigned	d = zombies[s].delay + zombies[s].speed;
				zombies[s].delay = d;
				
				// Carry over into position MSB
				if (d & 0x0100)
				{
					if (zombies[s].type == ZOMBIE_DANCER)
						zombie_sync_backup(y, s);

					if (zombies[s].type == ZOMBIE_DANCER && zombies[s].extra == 5)
					{
						if (zombies[s].phase == 0)
							zombie_add_backup(y, s);
					}
					else if (zombies[s].type == ZOMBIE_RESURRECT || zombies[s].type == ZOMBIE_BACKUP_RAISE)
					{

					}
					else if (zombies[s].type == ZOMBIE_BACKUP && zombies[s].extra != 0xff)
					{
						char dx = zombies[zombies[s].extra].x;
						char x = zombies[s].x;
						if (x + 4 < dx)
							x = dx - 16;
						else if (x > dx + 4)
							x = dx + 16;
						else
							x = dx;
						zombies[s].x = x;
					}
					else
						zombies[s].x --;

					// Animate zombie
					zombies[s].phase++;
					if (zombies[s].phase >= 6)
					{
						zombies[s].phase = 0;
						if (zombies[s].type == ZOMBIE_DANCER)
						{
							zombies[s].extra++;
							if (zombies[s].extra == 6)
							{
								zombies[s].extra = 0;
							}
						}
						else if (zombies[s].type == ZOMBIE_RESURRECT)
						{
							zombies[s].type = ZOMBIE_BASE;
						}
						else if (zombies[s].type == ZOMBIE_BACKUP_RAISE)
						{
							zombies[s].type = ZOMBIE_BACKUP;
						}
					}
				}
			}

			if (px < 9)
			{
				// Eaten by chomper
				if (plant_grid[y][px].type == PT_CHOMPER_0 || plant_grid[y][px].type == PT_CHOMPER_1)
				{
					zombies[s].type = ZOMBIE_NONE;
					plant_grid[y][px].type = PT_CHOMPER_EAT_0;
					plant_grid[y][px].cool = 150;
					plant_draw(px, y);	
				}
				else if (px > 0 && (plant_grid[y][px - 1].type == PT_CHOMPER_0 || plant_grid[y][px - 1].type == PT_CHOMPER_1))
				{
					zombies[s].type = ZOMBIE_NONE;
					plant_grid[y][px - 1].type = PT_CHOMPER_EAT_0;
					plant_grid[y][px - 1].cool = 150;
					plant_draw(px - 1, y);	
				}
			}
		}

		if (zombies[s].type != ZOMBIE_NONE)
		{
			if (zombies[s].x == 0)
				lost = true;

			// Check for emergency lawn mower
			if (zombies[s].x < 12 && mower_start(y))
				zombies[s].x = 12;

			// Update zombie bounds
			if (zombies[s].x < left)
				left = zombies[s].x;
			if (zombies[s].x > right)
				right = zombies[s].x;

			// Check sprite animation
			if (nz < ZOMBIE_SPRITES)
			{
				unsigned	x = zombies[s].x << 1;
				char		img = zombies[s].phase;

				switch (zombies[s].type)
				{
					case ZOMBIE_CORPSE:
				 		img += 100 + 16;
				 		break;
				 	case ZOMBIE_BASE:
				 	case ZOMBIE_BACKUP:
				 		img += 0 + 16;
				 		break;
				 	case ZOMBIE_CONE:
				 		img += 10 + 16;
				 		break;
				 	case ZOMBIE_POLE:
				 		img += 20 + 16;
				 		break;
				 	case ZOMBIE_VAULT:
				 		img += 26 + 16;
				 		break;
				 	case ZOMBIE_BUCKET:
				 		img += 30 + 16;
				 		break;
				 	case ZOMBIE_PAPER:
				 		img += 40 + 16;
				 		break;
				 	case ZOMBIE_ANGRY:
				 		img += 50 + 16;
				 		break;
				 	case ZOMBIE_SCREENDOOR:
				 		img += 60 + 16;
				 		break;
				 	case ZOMBIE_FOOTBALL:
				 		img += 70 + 16;
				 		break;
				 	case ZOMBIE_DANCER:
				 		img += 80 + 16;
				 		break;
				 	case ZOMBIE_BACKUP_RAISE:
				 	case ZOMBIE_RESURRECT:
				 		img += 90 + 16;
				 		break;
				}

				// Update multiplexer
				rirq_data(zombieMux[y], 1 * ZOMBIE_SPRITES + nz, x & 0xff);
				rirq_data(zombieMux[y], 2 * ZOMBIE_SPRITES + nz, img);
				rirq_data(zombieMux[y], 3 * ZOMBIE_SPRITES + nz, color);

				// Update MSB
				if (x & 0x100)
					msbx |= 1 << nz;
				nz++;
			}

			p = s;
		}
		else
		{
			// Remove zombie from list
			zombies_count[y]--;
			if (p == 0xff)
				zombies_first[y] = n;
			else
				zombies[p].next = n;
			zombies[s].next = zombies_free;
			zombies_free = s;

		}

		s = n;
	}

	// Fill remaining sprite slots
	while (nz < ZOMBIE_SPRITES)
	{
		rirq_data(zombieMux[y], 1 * ZOMBIE_SPRITES + nz, 0);
		nz++;
	}

	// Set zombie msb
	zombies_msbx[y] = msbx;
	rirq_data(zombieMux[y], 4 * ZOMBIE_SPRITES, msbx | zombies_basemsbx);

	zombies_left[y] = left;
	zombies_right[y] = right;

	return lost;
}

bool zombies_done(void)
{
	#pragma unroll(full)
	for(char i=0; i<5; i++)
		if (zombies_first[i] != 0xff)
			return false;
	return true;
}
