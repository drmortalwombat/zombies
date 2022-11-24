#ifndef LAWNMOWER_H
#define LAWNMOWER_H

extern char		mowerX, mowerY;
extern bool		mowers[5];

bool mower_start(char y);

bool mower_advance(void);

void mower_init(void);

#pragma compile("lawnmower.c")

#endif
