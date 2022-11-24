#ifndef ZOMBIES_H
#define ZOMBIES_H

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
	ZOMBIE_FOOTBALL
};

struct Zombie
{
	char		x;
	char		phase;
	char		next;
	signed char	live;
	char		frozen;
	ZombieType	type;
	char		delay;
	char		speed;
};

extern __striped Zombie	zombies[32];
extern char		zombies_first[5], zombies_free, zombies_count[5];
extern char		zombies_left[5], zombies_right[5];

#pragma align(zombies, 256)

void zombies_init(void);

bool zombies_add(char x, char y, ZombieType type);

void zombies_grave(ZombieType type);

void zombies_set_msbx(char mask, char val);

void zombies_advance(char y);

void zombies_splash(char x, char y, char w, char damage);

void zombies_fume(char x, char y, char w);

bool zombies_done(void);

#pragma compile("zombies.c")

#endif
