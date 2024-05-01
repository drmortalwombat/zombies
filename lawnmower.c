#include "lawnmower.h"
#include "plants.h"
#include "zombies.h"
#include <c64/vic.h>
#include <audio/sidfx.h>

extern char		mowerX, mowerY;
bool			mowers[5];

SIDFX	SIDFXMower[2] = {{
	3000, 4096,
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_72,
	0xf0  | SID_DKY_300,
	-400, 0,
	2, 0,
	10
},{
	5000, 4096,
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_72,
	0xf0  | SID_DKY_300,
	-400, 0,
	2, 2,
	10
}};

void mower_init(void)
{
	// No mower moving, and all ready
	mowerX = mowerY = 0;
	for(char i=0; i<5; i++)
		mowers[i] = true;
}


bool mower_start(char y)
{
	// Check if a mower is already in progress
	if (mowerX == 0)
	{
		// Is mower in this row still available?
		if (!mowers[y])
			return false;

		mowers[y] = false;
	
		// Set this as the active mower
		mowerY = y;
		mowerX = 1;

		char sy = 50 + 49 + y * 32;

		// Init mower sprite
		spr_set(7, true, 12, sy, 16 + 104, VCOL_ORANGE, true, false, false);	
		zombies_set_msbx(0x80, 0x00);

		// Remove display of mower image on left border
		plant_clear_mower(y);
	}
	else if (y != mowerY && !mowers[y])
		return false;

	return true;
}

bool mower_advance(void)
{
	// Check if mower is moving
	if (mowerX > 0)
	{
		// Advance by four pixel
		mowerX += 2;
		if (mowerX > 170)
		{
			// Reached right border, we are done
			mowerX = 0;
			return false;
		}
		else
		{
			unsigned x = 12 + 2 * mowerX;
			char 	y = 50 + 49 + mowerY * 32;

			// Place mower sprite
			spr_move(7, x, y);
			zombies_set_msbx(0x80, (x & 0x100) != 0 ? 0x80 : 0x00);

			// Destroy all zombies at this mower position
			zombies_splash(mowerX, mowerY, 16, 30);

			// Play mower sound
			if ((mowerX & 15) == 3)
				sidfx_play(2, SIDFXMower, 2);

			return true;
		}		
	}
	else
		return false;
}
