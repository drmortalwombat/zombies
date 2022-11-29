#include "gamemusic.h"
#include <c64/sid.h>


#pragma section( music, 0)

#pragma region( music, 0x9800, 0xbe00, , , {music} )

#pragma data(music)

__export char music[] = {
	#embed 0x2600 0x88 "zombies.sid" 
};

#pragma data(data)

char		music_throttle;

void music_init(Tune tune)
{
	__asm
	{
		lda		tune
		jsr		$9800
	}
}

void music_queue(Tune tune)
{
}

void music_play(void)
{
#if 0
	if (ntsc)
	{
		music_throttle++;
		if (music_throttle == 6)
		{
			music_throttle = 0;
			return;
		}
	}
#endif
	if (music_active)
	{
		__asm
		{
			jsr		$9803
		}
	}
}

void music_patch_voice3(bool enable)
{
	*(char *)0x9968 = enable ? 0x20 : 0x4c;
}

void music_toggle(void)
{
}
