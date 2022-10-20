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
	char		live;
	ZombieType	type;
};

extern Zombie	zombies[32];
extern char		zombies_first[5], zombies_free;

void zombies_init(void);

void zombies_add(char x, char y, ZombieType type);

void zombies_set_msbx(char mask, char val);

void zombies_advance(char y);

#pragma compile("zombies.c")

#endif