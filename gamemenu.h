#ifndef GAMEMENU_H
#define GAMEMENU_H

#include "display.h"

char gamemenu_query(const char ** menu);

enum GameResponse {
	GMENU_CONTINUE,
	GMENU_RESTART,
	GMENU_EXIT,
	
	GMENU_START,

	GMENU_COMPLETED,
	GMENU_FAILED
};

GameResponse gamemenu_ingame(void);


#pragma compile("gamemenu.c")

#endif
