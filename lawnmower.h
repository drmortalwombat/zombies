#ifndef LAWNMOWER_H
#define LAWNMOWER_H

// Position of lawn mower
extern char		mowerX, mowerY;

// Flags for availabel lawn mowers
extern bool		mowers[5];

// Start a lawn mower in the given row
bool mower_start(char y);

// Advance the lawn mower
bool mower_advance(void);

// Initialize the emergency lawn mowers
void mower_init(void);

#pragma compile("lawnmower.c")

#endif
