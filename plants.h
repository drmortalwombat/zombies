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

	PT_SUN,
	PT_SHOVEL,

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
	char		pad;
};

struct MenuItem
{
	PlantType	type;
	unsigned	price;
	char		cool, warm;
};

extern Plant	plant_grid[5][10];
extern char		plant_first[5];
extern Shot		shots[32];
extern char		shots_first, shots_free;
extern MenuItem	menu[10];
extern char		menu_size;
extern int 		sun_x, sun_y, sun_vx, sun_vy;
extern bool		sun_active;
extern char		sun_power;

#pragma align(plant_grid, 256)
#pragma align(shots, 256)

void sun_advance(void);

void sun_add(char x, char y, char vy, char power);

void menu_draw(char x, char t);

void menu_draw_price(char x, unsigned v);

void menu_init(void);

void menu_cooldown(char x);

void menu_warmup(void);

void menu_add_item(PlantType type, unsigned price, char warm);

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
