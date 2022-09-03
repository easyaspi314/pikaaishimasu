#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "global.h"

// Graphic data


#define BG_SIZE_U32 ((SCREEN_WIDTH * SCREEN_HEIGHT) / sizeof(u32)) // 9600

extern const u32 BG_Disclaimer[BG_SIZE_U32];
extern const u32 DAT_0800bc9c[4800];
extern const u32 DAT_0801079c[2560];
extern const u32 DAT_08012f9c[2560];
extern const u32 DAT_0801579c[2560];
extern const u32 BG_Stage1[BG_SIZE_U32];
extern const u32 BG_Stage2[BG_SIZE_U32];
extern const u32 BG_Stage3[BG_SIZE_U32];
extern const u32 BG_LevelName[BG_SIZE_U32];
extern const u32 BG_LevelClear[BG_SIZE_U32];
extern const u32 BG_GameOver[BG_SIZE_U32];
extern const u32 DAT_0805039c[1280];
extern const u32 DAT_0805179c[768];
extern const u32 Sprite_Pikachu[4096];
extern const u32 BG_Copyright[BG_SIZE_U32];

// Palette format is RGB triplets of 0-31. I don't know how
// they made this so weird
// WEIRDNESS: For some reason these palettes all have 257 entries.
#define PALETTE_SIZE (257 * 3)

extern const u8  Palette_DisclaimerAndCopyright[PALETTE_SIZE];
extern const u8  DAT_0805fc9f[PALETTE_SIZE];
extern const u8  DAT_0805ffa2[PALETTE_SIZE];
extern const u8  Palette_Stage1[PALETTE_SIZE];
extern const u8  Palette_Stage2[PALETTE_SIZE];
extern const u8  Palette_Stage3[PALETTE_SIZE];
extern const u8  Palette_LevelName[PALETTE_SIZE];
extern const u8  Palette_LevelClear[PALETTE_SIZE];
extern const u8  Palette_GameOver[PALETTE_SIZE];

// These values are in writable memory for no good reason.
// Pointless pointers
extern const u32 *CONSTANT PTR_BG_Disclaimer;
extern const u32 *CONSTANT PTR_BG_Copyright;
extern const u8 *CONSTANT PTR_Palette_DisclaimerAndCopyright;

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