#ifndef GAMEMENU_H
#define GAMEMENU_H

#include "display.h"

// Show menu and have player select an entry
char gamemenu_query(const char ** menu);

enum GameResponse {
	GMENU_CONTINUE,
	GMENU_RESTART,
	GMENU_EXIT,
	
	GMENU_START,

	GMENU_COMPLETED,
	GMENU_FAILED
};

// Show and execute in game menu
GameResponse gamemenu_ingame(void);


#pragma compile("gamemenu.c")

#endif
