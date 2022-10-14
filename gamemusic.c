#include "gamemusic.h"
#include <c64/sid.h>


#pragma section( music, 0)

#pragma region( music, 0xa000, 0xb400, , , {music} )

#pragma data(music)

__export char music[] = {
	#embed 0x2000 0x88 "zombies.sid" 
};

#pragma data(data)

char		music_throttle;

void music_init(Tune tune)
{
	__asm
	{
		lda		tune
		jsr		$a000
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
	__asm
	{
		jsr		$a003
	}
}

void music_patch_voice3(bool enable)
{
	*(char *)0xa156 = enable ? 0x20 : 0x4c;
}

void music_toggle(void)
{
}
