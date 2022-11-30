#ifndef PLANTS_H
#define PLANTS_H

#pragma compile("plants.c")

enum PlantType
{
	PT_NONE_DAY,
	PT_NONE_NIGHT,

	PT_TOMBSTONE,
	PT_CRATER,

	PT_GROUND,

	PT_PEASHOOTER_0,
	PT_PEASHOOTER_1,
	PT_SUNFLOWER_0,
	PT_SUNFLOWER_1,
	PT_WALLNUT_0,
	PT_WALLNUT_1,
	PT_CHERRYBOMB,
	PT_TESTSHROOM,
	PT_CACTUS,
	PT_POTATOMINE_0,
	PT_POTATOMINE_1,
	PT_REPEATER_0,
	PT_REPEATER_1,

	PT_SUN,
	PT_SHOVEL,

	PT_SNOWPEA_0,
	PT_SNOWPEA_1,
	PT_POTATOMINE_HIDDEN,
	PT_POTATOMINE_EXPLODED,

	PT_EXPLOSION_0,
	PT_EXPLOSION_1,
	PT_EXPLOSION_2,
	PT_EXPLOSION_3,

	PT_CHOMPER_0,
	PT_CHOMPER_1,
	PT_CHOMPER_EAT_0,
	PT_CHOMPER_EAT_1,

	PT_PUFFSHROOM_0,
	PT_PUFFSHROOM_1,

	PT_SUNSHROOM_0,
	PT_SUNSHROOM_1,

	PT_SUNSHROOM_BIG_0,
	PT_SUNSHROOM_BIG_1,

	PT_FUMESHROOM_0,
	PT_FUMESHROOM_1,

	PT_GRAVEDIGGER_0,
	PT_GRAVEDIGGER,

	PT_SCAREDYSHROOM_0,
	PT_SCAREDYSHROOM_1,

	PT_SCAREDYSHROOM_DUCK,

	PT_ICESHROOM,
	PT_SNOWFLAKE,

	PT_DOOMSHROOM,
	PT_DOOMCLOUD,

	PT_CONVEYOR,
	PT_FLOORSPACE,
	PT_FLOORSPACE_MOWER,

	PT_BORDER,
	PT_CARDSLOT,
	PT_FREESLOT,
	PT_GO,

	NUM_PLANT_TYPES
};

struct Plant
{	
	char		next;
	PlantType	type;
	char		cool;
	char		live;
	char		age;
};

enum ShotType
{
	ST_PEA,
	ST_FROST,
	ST_PUFF,
	ST_FUME
};

struct Shot
{
	char		next;
	char		x, y, t;
	ShotType	type;
};

struct MenuItem
{
	PlantType	type;
	unsigned	price;
	char		cool, warm;
	bool		once;
};

extern Plant	plant_grid[5][10];
extern char		plant_first[5];
extern __striped Shot		shots[32];
extern char		shots_first, shots_free;
extern __striped MenuItem	menu[10];
extern char		menu_size, menu_first;
extern int 		sun_x, sun_y, sun_vx, sun_vy;
extern bool		sun_active;
extern char		sun_power;
extern char		back_color;
extern char		back_tile;
extern const char color_grey[16];

#pragma align(plant_grid, 256)
#pragma align(shots, 256)

void sun_init(void);

void sun_advance(void);

void sun_add(char x, char y, char vy, char power);

void menu_draw(char x, char t);

void menu_init(char slots);

void menu_cooldown(char x);

void menu_warmup(void);

void menu_add_item(PlantType type, unsigned price, char warm, bool ready, bool once);

void menu_remove_item(PlantType type);

void menu_add_item_at(char x, PlantType type, unsigned price, char warm, bool ready, bool once);

void menu_progress(char p, char m);

// x and y in plant coords
void plant_draw(char x, char y);

// x and y in screen coords
void plant_draw_field(char x, char y);

void plant_draw_borders(void);

void plant_clear_mower(char y);

void plant_grid_clear(char rows);

void plant_row_draw(char y);

void plant_grid_draw(void);

void plant_place(char x, char y, PlantType p);

void plant_remove(char x, char y);

void shots_init(void);

void shots_add(char x, char y, char t, ShotType type);

void shots_advance(char step);

void plants_iterate(char y);

void plants_animate(char y);

#endif
