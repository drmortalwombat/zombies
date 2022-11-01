#ifndef GAMEMUSIC_H
#define GAMEMUSIC_H

/*
00-Title-2:33
01-Tune1-2:24
*/

enum Tune
{
	TUNE_GAME_1,
	TUNE_GAME_2,
	TUNE_GAME_3,
	TUNE_TITLE,
	TUNE_SEEDS,
	TUNE_GAME_4, // NIGHT
	TUNE_GAME_5, // NIGHT
	TUNE_VICTORY,
	TUNE_GAME_6, // NIGHT
};

void music_init(Tune tune);

void music_queue(Tune tune);

void music_play(void);

void music_toggle(void);

void music_patch_voice3(bool enable);

#pragma compile("gamemusic.c")

#endif
