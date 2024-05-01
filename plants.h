#ifndef PLANTS_H
#define PLANTS_H

#include "display.h"

// Status for each plant slot on the lawn
struct Plant
{	
	char		next;	// Next occupied plant position in the row
	PlantType	type;	// Type of plant
	char		cool;	// Cooldown for shooting
	char		live;	// Remaining live
	char		age;	// Age of plant
};

// Type of shot
enum ShotType
{
	ST_PEA,
	ST_FROST,
	ST_PUFF,
	ST_FUME
};

// List node for shots in flight
struct Shot
{
	char		next;		// Index of next shot
	char		x, y, t;	// Position and age of shot
	ShotType	type;		// Type of shot
};

// Menu item for seed items
struct MenuItem
{
	PlantType	type;
	unsigned	price;
	char		cool, warm;
	bool		once;
};

// Plant slots
extern Plant				plant_grid[5][10];
// Index of first plant in each row
extern char					plant_first[5];

// Shot slots
extern __striped Shot		shots[32];
// List head of active and free shots
extern char					shots_first, shots_free;

extern __striped MenuItem	menu[10];
extern char					menu_size, menu_first;

// Sunshine in flight
extern int 					sun_x, sun_y, sun_vx, sun_vy;
extern bool					sun_active;
extern char					sun_power;
extern char					back_tile;

// Map from colour to greyscale
extern const char 			color_grey[16];

#pragma align(plant_grid, 256)
#pragma align(shots, 256)

// Initialize sun shine
void sun_init(void);

// Advanve sun shine counter
void sun_advance(void);

// Add a sun shine sprite
void sun_add(char x, char y, char vy, char power);

// Draw a menu slot
void menu_draw(char x, char t);

// Init the seed menu
void menu_init(char slots);

// Cooldown a menu entry
void menu_cooldown(char x);

// Warmup all not yet ready menu items
void menu_warmup(void);

// Add an item to the seed menu
void menu_add_item(PlantType type, unsigned price, char warm, bool ready, bool once);

// Remove an item from the seed menu
void menu_remove_item(PlantType type);

// Add an item at a fixed position to the seed menu
void menu_add_item_at(char x, PlantType type, unsigned price, char warm, bool ready, bool once);

void menu_progress(char p, char m);

// x and y in plant coords
void plant_draw(char x, char y);

// x and y in screen coords
void plant_draw_field(char x, char y);

// Draw left and right borders
void plant_draw_borders(void);

// Clear mower from border
void plant_clear_mower(char y);

// Clear grid
void plant_grid_clear(char rows);

// Draw a row of plants
void plant_row_draw(char y);

// Draw the full grid of plants
void plant_grid_draw(void);

// Place a plant at a given grid location
void plant_place(char x, char y, PlantType p);

// Remove a plant from a grid location
void plant_remove(char x, char y);

// Initialize shots
void shots_init(void);

// Add a shot
void shots_add(char x, char y, char t, ShotType type);

// Advance all shots by "step" time slots
void shots_advance(char step);

// Iterate plants in the given row
void plants_iterate(char y);

// Animate plants in the given row
void plants_animate(char y);

#pragma compile("plants.c")


#endif
