#ifndef ZOMBIES_H
#define ZOMBIES_H

enum ZombieType
{
	ZT_BASE,
	ZT_HEAD
};

struct Zombie
{
	char		y;
	char		x;
	char		phase;
	char		next;
	signed char	live;
	char		frozen;
	ZombieType	type;
	char		pad0;
};

extern Zombie	zombies[32];
extern char		zombies_first[5], zombies_free;
extern char		zombies_left[5], zombies_right[5];

#pragma align(zombies, 256)

void zombies_init(void);

void zombies_add(char x, char y, ZombieType type);

void zombies_set_msbx(char mask, char val);

void zombies_advance(char y);

#pragma compile("zombies.c")

#endif
