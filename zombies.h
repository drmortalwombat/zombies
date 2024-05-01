#ifndef ZOMBIES_H
#define ZOMBIES_H

// Type of zombies
enum ZombieType
{
	ZOMBIE_NONE,
	ZOMBIE_CORPSE,

	ZOMBIE_BASE,
	ZOMBIE_CONE,
	ZOMBIE_POLE,
	ZOMBIE_VAULT,
	ZOMBIE_BUCKET,
	ZOMBIE_PAPER,
	ZOMBIE_ANGRY,
	ZOMBIE_SCREENDOOR,
	ZOMBIE_FOOTBALL,
	ZOMBIE_DANCER,
	ZOMBIE_BACKUP,
	ZOMBIE_BACKUP_RAISE,
	ZOMBIE_RESURRECT
};

struct Zombie
{
	char		x;		// x position in two pixels
	char		phase;	// animation phase
	char		next;	// linked list
	signed char	live;	// remaining (un)live
	char		frozen;	// freeze conunter
	ZombieType	type;	// type of zombie
	char		delay;	// low byte position integrator
	char		speed;	// speed
	char		extra;	// some type specific info
};

// Zombie slots
extern __striped Zombie	zombies[32];
// Zombie lists per row
extern char		zombies_first[5], zombies_free, zombies_count[5];
// Zombie limit per row
extern char		zombies_left[5], zombies_right[5];

#pragma align(zombies, 256)

// Init zombie lists
void zombies_init(void);

// Clear all zombies
void zombies_clear(void);

// Add a zombie to the row if possible
bool zombies_add(char x, char y, ZombieType type, char extra);

// Resurrect zombies from the graves
void zombies_grave(ZombieType type);

// Set sprite MSB for non zombie sprites
void zombies_set_msbx(char mask, char val);

// Advance all zombies in this row
bool zombies_advance(char y);

// Splashdamage to zombies around this tile
void zombies_splash(char x, char y, char w, char damage);

// Fume damage to zombies around this tile
void zombies_fume(char x, char y, char w);

// Freeze all zombies
void zombies_freeze_all(char frost);

// Check if done with zombies
bool zombies_done(void);

#pragma compile("zombies.c")

#endif
