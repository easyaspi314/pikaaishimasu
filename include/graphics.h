#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "global.h"

// Graphic data


#define BG_SIZE_U32 ((SCREEN_WIDTH * SCREEN_HEIGHT) / sizeof(u32)) // 9600

extern const u32 IMG_Disclaimer[BG_SIZE_U32];
extern const u32 IMG_TitleScreenName[4800];
extern const u32 IMG_TitleScreenGrass[2560];
extern const u32 IMG_TitleScreenTrees[2560];
extern const u32 IMG_TitleScreenCloud[2560];
extern const u32 IMG_Level1[BG_SIZE_U32];
extern const u32 IMG_Level2[BG_SIZE_U32];
extern const u32 IMG_Level3[BG_SIZE_U32];
extern const u32 IMG_LevelName[BG_SIZE_U32];
extern const u32 IMG_LevelClear[BG_SIZE_U32];
extern const u32 IMG_GameOver[BG_SIZE_U32];
extern const u32 IMG_Font[1280];
extern const u32 IMG_Balls[768];
extern const u32 IMG_Pikachu[4096];
extern const u32 IMG_Copyright[BG_SIZE_U32];

// Palette format is RGB triplets of 0-31. I don't know why
// they made this so weird
#define PALETTE_SIZE (256 * 3)

extern const u8  PAL_Disclaimer[PALETTE_SIZE];
extern const u8  PAL_TitleScreenName[PALETTE_SIZE];
extern const u8  PAL_Pikachu[PALETTE_SIZE];
extern const u8  PAL_Level1[PALETTE_SIZE];
extern const u8  PAL_Level2[PALETTE_SIZE];
extern const u8  PAL_Level3[PALETTE_SIZE];
extern const u8  PAL_LevelName[PALETTE_SIZE];
extern const u8  PAL_LevelClear[PALETTE_SIZE];
extern const u8  PAL_GameOver[PALETTE_SIZE];

// These values are in writable memory for no good reason.
// Pointless pointers
extern const u32 *CONSTANT PTR_IMG_Disclaimer;
extern const u32 *CONSTANT PTR_IMG_Copyright;
extern const u8 *CONSTANT PTR_PAL_Disclaimer;

// Indices for the player sprite
extern u8 CONSTANT gPlayerSpriteIndexesMoving[32];
extern u8 CONSTANT gPlayerSpriteIndexesStill[32];

// timer for rng
extern vu16 *CONSTANT PTR_TIMER_RNG;
extern vu16 *CONSTANT PTR_REG_VCOUNT;
extern u16 *CONSTANT PTR_BG_COLORS;
extern u16 *CONSTANT PTR_OBJ_COLORS;

// Video ram pointers
// These don't match if they are arrays
extern u32 *CONSTANT PTR_FRONTBUFFER;
extern u32 *CONSTANT PTR_BACKBUFFER;
extern u32 *CONSTANT PTR_OBJ_TILES;
extern u16 *CONSTANT PTR_OAM;
extern u32 *CONSTANT PTR_TILE_BASES_0;
extern u32 *CONSTANT PTR_TILE_BASES_1;
extern u32 *CONSTANT PTR_TILE_BASES_2;
extern u32 *CONSTANT PTR_TILE_BASES_3;
extern u16 *CONSTANT PTR_MAP_10;
extern u16 *CONSTANT PTR_MAP_11;
extern u16 *CONSTANT PTR_MAP_12;
extern u16 *CONSTANT PTR_MAP_13;


#endif // GRAPHICS_H
