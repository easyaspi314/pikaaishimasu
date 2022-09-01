u32 *PTR_VRAM_03000000 = (u32 *)MODE5_BB;   // referenced but unused 
u32 *PTR_VRAM_03000004 = (u32 *)MODE5_FB;
u32 *PTR_VRAM_03000008 = (u32 *)MODE5_BB;
// timer for rng
vu16 *pTimerRNG = &REG_TM0CNT_L;
vu16 *PTR_REG_VCOUNT = &REG_VCOUNT;
u16 *PTR_BG_COLORS = BG_COLORS;
u16 *PTR_OBJ_COLORS = OBJ_COLORS;
u32 *PTR_VRAM_0300001c = (u32 *)OBJ_BASE_ADR;

// OBJATTR *PTR_OAM = OAM;
u16 *PTR_OAM = (u16 *)OAM;
u32 *PTR_VRAM_03000024 = (u32 *)VRAM;
u16 *PTR_VRAM_03000028 = (u16 *)TILE_BASE_ADR(1);
u32 *PTR_VRAM_0300002c = (u32 *)TILE_BASE_ADR(2);
u32 *PTR_VRAM_03000030 = (u32 *)TILE_BASE_ADR(3);
u16 *PTR_VRAM_03000034 = (u16 *)MAP_BASE_ADR(10);
u16 *PTR_VRAM_03000038 = (u16 *)MAP_BASE_ADR(11);
u16 *PTR_VRAM_0300003c = (u16 *)MAP_BASE_ADR(12);
u16 *PTR_VRAM_03000040 = (u16 *)MAP_BASE_ADR(13);
u16 *PTR_VRAM_03000044 = (u16 *)(VRAM + 0x8000);  // unused
u16 *PTR_VRAM_03000048 = (u16 *)(VRAM + 0x9800);  // unused
u16 *PTR_VRAM_0300004c = (u16 *)(VRAM + 0xa000);  // unused
// ah yes, global variable loop counters :P
u16 gI = 0;
u16 gJ = 0;
u32 unused_03000058 = 0;
// not sure why these are here
const u32 *pDAT_0800269c = DAT_0800269c;
const u32 *pDAT_0805639c = DAT_0805639c;
const u8 *pDAT_0805f99c = DAT_0805f99c;
u32 unused_03000068 = 0;
#define MINIMUM_SCORE 150
u16 gCurrentScore = 0;
u8 gCurrentStage = 1;
#define MAXIMUM_POINTS 250
s16 gRemainingPoints = 0x000a;
u8 DAT_03000078 = 0x00;
u8 gPlayerX = 104;
u8 gPlayerY = 112;
#define PLAYER_DIRECTION_LEFT false
#define PLAYER_DIRECTION_RIGHT true
bool gPlayerDirection = PLAYER_DIRECTION_LEFT;
bool gPlayerIsMoving = false;
u8 gPlayerWasMoving = true;
u8 gPlayerSpriteFrame = 0x00;
u8 gFrameCycle = 0x00;
u8 DAT_03000098 = 0x00;
u8 DAT_0300009c[] = {
    0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03,
    0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03,
    0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03,
    0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03
};
u8 DAT_030000bc[] = {
    0x04, 0x05, 0x06, 0x07, 0x04, 0x05, 0x06, 0x07,
    0x04, 0x05, 0x06, 0x07, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08
};
u32 DAT_030000dc = 0;
// bss
#define NUM_BALLS 10
struct BallState {
    u8 y[NUM_BALLS];
    u8 x[NUM_BALLS];
    u8 speed[NUM_BALLS];
    u8 types[NUM_BALLS];
    u8 rotation[NUM_BALLS];
    u8 field_5[NUM_BALLS];
    u8 field_6[NUM_BALLS];
};
struct BallState gBallState;
#define POKE_BALL 0
#define GREAT_BALL 1
#define ULTRA_BALL 2

#define FRAME_SKIP 4