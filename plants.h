#ifndef PLANTS_H
#define PLANTS_H

#pragma compile("plants.c")

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
	char		cool;
	char		live;
};

struct Shot
{
	char		next;
	char		x, y;
};

extern Plant	plant_grid[5][10];
extern char		plant_first[5];
extern Shot		shots[32];
extern char		shots_first, shots_free;

void menu_draw(char x, char t);

void menu_draw_price(char x, unsigned v);

// x and y in plant coords
void plant_draw(char x, char y);

// x and y in screen coords
void plant_draw_field(char x, char y);

void plant_grid_clear(void);

void plant_place(char x, char y, PlantType p);

void plant_remove(char x, char y);

void shots_init(void);

void shots_add(char x, char y);

void shots_advance(void);

void plants_iterate(char y);

#endif
