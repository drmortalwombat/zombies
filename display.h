#ifndef DISPLAY_H
#define DISPLAY_H

#include <c64/vic.h>
#include <c64/sprites.h>
#include <c64/rasterirq.h>
#include <c64/memmap.h>

static char * const Screen	=	(char *)0xc000;
static char * const Color	=	(char *)0xd800; 
static char * const Sprites =  (char *)0xc400;
static char * const Hires	=	(char *)0xe000;

#define ZOMBIE_SPRITES	6

extern RIRQCode	*	zombieMux[5];
extern RIRQCode		menuMux, cursorMux;

extern __striped char * const HiresTab[25];
extern __striped char * const ColorTab[25];
extern __striped char * const ScreenTab[25];

void display_init(void);

void set_cursor_sprite(char x, char y);

void text_put(char x, char y, char c, const char * t);

void disp_put_price(unsigned v, char dx, char dy);

void disp_put_noprice(char dx, char dy);

void disp_color_price(char dx, char dy);

#pragma compile("display.c")

#endif

