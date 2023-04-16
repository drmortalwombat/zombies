#include "gamemenu.h"
#include <c64/keyboard.h>
#include <c64/joystick.h>
#include <c64/vic.h>

char gamemenu_query(const char ** menu)
{
	char	n = 0;
	char	y = 8;
	char	x = 10;

	while (menu[n])
	{
		text_put(x, y + 2 * n, n == 0 ? VCOL_YELLOW : VCOL_MED_GREY, menu[n]);
		n++;	
	}

	char spe = vic.spr_enable;
	vic.spr_enable = 0;

	bool	joydown = true;
	char	joymove = 0;

	char	s = 0;
	for(;;)
	{
		keyb_poll();
		switch (keyb_key)
		{
			case KSCAN_CSR_DOWN | KSCAN_QUAL_DOWN:
			case KSCAN_S | KSCAN_QUAL_DOWN:
				if (s + 1 < n)
				{
					text_put(x, y + 2 * s, VCOL_MED_GREY, menu[s]);
					s++;
					text_put(x, y + 2 * s, VCOL_YELLOW, menu[s]);
				}
				break;
			case KSCAN_CSR_DOWN | KSCAN_QUAL_SHIFT | KSCAN_QUAL_DOWN:
			case KSCAN_W | KSCAN_QUAL_DOWN:
				if (s > 0)
				{
					text_put(x, y + 2 * s, VCOL_MED_GREY, menu[s]);
					s--;
					text_put(x, y + 2 * s, VCOL_YELLOW, menu[s]);
				}
				break;
			case KSCAN_SPACE | KSCAN_QUAL_DOWN:
			case KSCAN_RETURN | KSCAN_QUAL_DOWN:

				vic.spr_enable = spe;

				return s;
				break;			
		}

		joy_poll(0);
		if (joydown)
		{
			if (!joyb[0])
				joydown = false;
		}
		else if (joyb[0])
		{
			vic.spr_enable = spe;
			return s;
		}

		if (joymove)
		{
			joymove--;
			if (!joyy[0])
				joymove = 0;
		}
		else if (joyy[0] < 0 && s > 0)
		{
			text_put(x, y + 2 * s, VCOL_MED_GREY, menu[s]);
			s--;
			text_put(x, y + 2 * s, VCOL_YELLOW, menu[s]);
			joymove = 20;
		}
		else if (joyy[0] > 0 && s + 1 < n)
		{
			text_put(x, y + 2 * s, VCOL_MED_GREY, menu[s]);
			s++;
			text_put(x, y + 2 * s, VCOL_YELLOW, menu[s]);
			joymove = 20;
		}

		vic_waitFrame();
	}
}

const char * ingame_menu[] = {
	"CONTINUE",
	"RESTART",
	"EXIT",
	nullptr
};

GameResponse gamemenu_ingame(void)
{
	return gamemenu_query(ingame_menu);
}
