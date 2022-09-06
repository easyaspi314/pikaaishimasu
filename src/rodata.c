// This program was compiled in one translation unit, aside from the asm code startup. 


#include "../graphics/backgrounds/IMG_Disclaimer.c"
#include "../graphics/title_screen/IMG_TitleScreenName.c"
#include "../graphics/title_screen/IMG_TitleScreenGrass.c"
#include "../graphics/title_screen/IMG_TitleScreenTrees.c"
#include "../graphics/title_screen/IMG_TitleScreenCloud.c"
#include "../graphics/backgrounds/IMG_Level1.c"
#include "../graphics/backgrounds/IMG_Level2.c"
#include "../graphics/backgrounds/IMG_Level3.c"
#include "../graphics/backgrounds/IMG_LevelName.c"
#include "../graphics/backgrounds/IMG_LevelClear.c"
#include "../graphics/backgrounds/IMG_GameOver.c"
#include "../graphics/sprites/IMG_Font.c"
#include "../graphics/sprites/IMG_Balls.c"
#include "../graphics/sprites/IMG_Pikachu.c"
#include "../graphics/backgrounds/IMG_Copyright.c"


// Whatever exporter the developer used seemed to have read out of bounds and emitted 3 extra bytes
// for each palette
#ifdef BUGFIX
#  define PAL_OVERFLOW(addr, x, y, z) // nothing
#else
#  define PAL_OVERFLOW(addr, x, y, z) const unsigned char palette_overflow_##addr[3] = { x, y, z };
#endif
#include "../graphics/backgrounds/PAL_Disclaimer.c"
PAL_OVERFLOW(0805fc9c, 0x1D, 0x1a, 0x1e)
#include "../graphics/title_screen/PAL_TitleScreenName.c" // all title screen tiles 
PAL_OVERFLOW(0805ff9f, 0x00, 0x00, 0x00)
#include "../graphics/sprites/PAL_Pikachu.c" // also balls and fonts
PAL_OVERFLOW(080602a2, 0x00, 0x00, 0x00)
#include "../graphics/backgrounds/PAL_Level1.c"
PAL_OVERFLOW(080605a5, 0x05, 0x06, 0x06)
#include "../graphics/backgrounds/PAL_Level2.c"
PAL_OVERFLOW(080608a8, 0x00, 0x00, 0x00)
#include "../graphics/backgrounds/PAL_Level3.c"
PAL_OVERFLOW(08060bab, 0x00, 0x00, 0x00)
#include "../graphics/backgrounds/PAL_LevelName.c"
PAL_OVERFLOW(08060eae, 0x00, 0x00, 0x00)
#include "../graphics/backgrounds/PAL_LevelClear.c"
PAL_OVERFLOW(080611b1, 0x00, 0x00, 0x00)
#include "../graphics/backgrounds/PAL_GameOver.c"
#ifndef BUGFIX
const unsigned char palette_overflow_080614b4[4] = { 0, 0, 0, 0 };
#endif
