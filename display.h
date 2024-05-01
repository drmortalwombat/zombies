#ifndef DISPLAY_H
#define DISPLAY_H

#include <c64/vic.h>
#include <c64/sprites.h>
#include <c64/rasterirq.h>
#include <c64/memmap.h>

// Memory region for tile screen
static char * const Screen	=	(char *)0xc000;
// Memory region for color
static char * const Color	=	(char *)0xd800; 
// Memory region for sprites
static char * const Sprites =  (char *)0xc400;
// Memory region for bitmap
static char * const Hires	=	(char *)0xe000;

// Number of zombie sprites per row
#define ZOMBIE_SPRITES	6

enum PlantType
{
	PT_NONE_DAY_0,
	PT_NONE_DAY_1,
	PT_NONE_NIGHT_0,
	PT_NONE_NIGHT_1,

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

	PT_FENCE,

	NUM_PLANT_TYPES
};

extern RIRQCode	*	zombieMux[5];
extern RIRQCode		menuMux, cursorMux;

// Tables of pointers to screen/color/bitmap rows
extern __striped char * const HiresTab[25];
extern __striped char * const ColorTab[25];
extern __striped char * const ScreenTab[25];

extern char		back_color;

// Initialize display
void display_init(void);

// Move the selection cursor sprite
void set_cursor_sprite(char x, char y);

// Draw single colour text line
void text_put(char x, char y, char c, const char * t);

// Draw two colour text line
void text_put_2(char x, char y, char color1, char color2, const char * t);

// Draw a price bar for a menu entry
void disp_put_price(unsigned v, char dx, char dy);

// Draw a blank bar for a menu entry
void disp_put_noprice(char dx, char dy);

// Colorize the price bar
void disp_color_price(char dx, char dy);


void disp_put_char(PlantType type, char sx, char sy, char dx, char dy);

// Draw a background tile
void disp_put_tile(PlantType type, char dx, char dy);

// Draw a ghosted background tile
void disp_ghost_tile(PlantType type, char dx, char dy);

// Colorize a ghosted background tile
void disp_color_tile(PlantType type, char dx, char dy);

#pragma compile("display.c")

#endif

