#include "global.h"
#include "constants.h"
#include "graphics.h"
// The order of these variables is important.
u32 *PTR_BACKBUFFER_UNUSED = (u32 *)MODE5_BB; // referenced but unused
u32 *CONSTANT PTR_FRONTBUFFER = (u32 *)MODE5_FB;
u32 *CONSTANT PTR_BACKBUFFER = (u32 *)MODE5_BB;
// timer for rng
vu16 *CONSTANT PTR_TIMER_RNG = &REG_TM0CNT_L;
vu16 *CONSTANT PTR_REG_VCOUNT = &REG_VCOUNT;
u16 *CONSTANT PTR_BG_COLORS = BG_COLORS;
u16 *CONSTANT PTR_OBJ_COLORS = OBJ_COLORS;
// BUG: This bug makes the game unplayable on any emulator that is remotely
// accurate.
//
// The main game runs in Mode 4, a bitmap mode. In this mode, only sprites
// 512-1023 are displayed, as the first half of the object memory is reserved
// for the sprites. Everything else is invisible.
//
// This game tries to display sprites in the low 512 indices, which is not
// displayed on hardware or any accurate emulator, meaning you get to see a
// very pretty background...and nothing else.
#ifdef BUGFIX
u32 *CONSTANT PTR_OBJ_TILES = (u32 *)BITMAP_OBJ_BASE_ADR;
#else
u32 *CONSTANT PTR_OBJ_TILES = (u32 *)OBJ_BASE_ADR;
#endif
// OBJATTR *PTR_OAM = OAM;
u16 *CONSTANT PTR_OAM = (u16 *)OAM;
// array doesn't seem to match
u32 *CONSTANT PTR_TILE_BASES_0 = (u32 *)TILE_BASE_ADR(0);
u32 *CONSTANT PTR_TILE_BASES_1 = (u32 *)TILE_BASE_ADR(1); // unused
u32 *CONSTANT PTR_TILE_BASES_2 = (u32 *)TILE_BASE_ADR(2);
u32 *CONSTANT PTR_TILE_BASES_3 = (u32 *)TILE_BASE_ADR(3);

u16 *CONSTANT PTR_MAP_10 = (u16 *)MAP_BASE_ADR(10);
u16 *CONSTANT PTR_MAP_11 = (u16 *)MAP_BASE_ADR(11);
u16 *CONSTANT PTR_MAP_12 = (u16 *)MAP_BASE_ADR(12);
u16 *CONSTANT PTR_MAP_13 = (u16 *)MAP_BASE_ADR(13);
u16 *CONSTANT PTR_VRAM_03000044 = (u16 *)(VRAM + 0x8000); // unused
u16 *CONSTANT PTR_VRAM_03000048 = (u16 *)(VRAM + 0x9800); // unused
u16 *CONSTANT PTR_VRAM_0300004c = (u16 *)(VRAM + 0xa000); // unused

// ah yes, global variable loop counters :P
u16 gI = 0;
u16 gJ = 0;
u32 unused_03000058 = 0;
// not sure why these are here
const u32 *CONSTANT PTR_BG_Disclaimer = BG_Disclaimer;
const u32 *CONSTANT PTR_BG_Copyright = BG_Copyright;
const u8 *CONSTANT PTR_Palette_DisclaimerAndCopyright = Palette_DisclaimerAndCopyright;

u32 unused_03000068 = 0; // unused
u16 gCurrentScore = 0;
u8 gCurrentStage = 1;
s16 gRemainingPoints = 10; // ok then
s8 gBallCount = 0;
u8 gPlayerX = DEFAULT_PLAYER_X;
u8 gPlayerY = DEFAULT_PLAYER_Y;
bool gPlayerDirection = PLAYER_DIRECTION_LEFT;
bool gPlayerIsMoving = false;
bool gPlayerWasMoving = true;
u8 gPlayerSpriteFrame = 0;
u8 gFrameCycle = 0;
u8 gPlayerSpriteIndex = 0;
u8 CONSTANT gPlayerSpriteIndexesMoving[32] = {
    0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3,
    0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3
};
u8 CONSTANT gPlayerSpriteIndexesStill[32] = {
    4, 5, 6, 7, 4, 5, 6, 7, 4, 5, 6, 7, 4, 5, 6, 7,
    8, 9, 10, 11, 12, 13, 14, 15, 15, 14, 13, 12, 11, 10, 9, 8
};

// in bss
u8 gBallState[NUM_BALL_FIELDS][NUM_BALLS];
