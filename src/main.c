#include "global.h"
#include "graphics.h"
#include "constants.h"
#include "variables.h"

// This program was compiled in one translation unit, but that would be messy.
// Everything is declared beforehand.
#ifdef ONE_TRANSLATION_UNIT
#include "rodata.c"
#include "global_variables.c"
#endif

#define ArraySize(x) (sizeof(x) / sizeof((x)[0]))

// BUG: this doesn't advance the pointer correctly and ends up filling everything
// in OAM with 0x20c8, aside from the last entries. It has no visible effect though
// because it will try and display sprite 200 whch
#ifdef BUGFIX
#define BUGGED_OAM_INCREMENT 4
#else
#define BUGGED_OAM_INCREMENT 1
#endif

#define Inline_ResetSprites()                              \
    do                                                     \
    {                                                      \
        for (gI = 0; gI < 128; gI += BUGGED_OAM_INCREMENT) \
        {                                                  \
            PTR_OAM[gI + 0] = 200 | OBJ_256_COLOR;         \
            PTR_OAM[gI + 1] = 250;                         \
            PTR_OAM[gI + 2] = 0;                           \
        }                                                  \
    } while (0)

#define BallScore(ball) ((ball)*2 + 1)

// match
u16 RGB8ToRGB5(u8 r, u8 g, u8 b)
{
    return RGB5(r, g, b);
}
// match
void WaitForVBlank(void)
{
    // must be volatile even on armcc
    volatile u16 *pVCount = (volatile u16 *)&REG_VCOUNT;
    do
    {
    } while (*pVCount != SCREEN_HEIGHT);
}
// match
void CPUCopy32(u32 length, const u32 *src, u32 *dst)
{
    u32 i;
    for (i = 0; i < length; ++i)
    {
        dst[i] = src[i];
    }
}

// only matches as a macro or copy paste
#define Inline_UnkMatrixCopy(in, out, x, y)             \
    do                                                  \
    {                                                   \
        for (i = 0; i < 8; i++)                         \
        {                                               \
            for (j = 0; j < 2; j++)                     \
            {                                           \
                out[j + i * 2 + x] = in[j + i * 4 + y]; \
            }                                           \
        }                                               \
    } while (0)
// I *think* this rotates the poke ball sprites.
// match
void FUN_08000198(const u32 *in, u32 *out)
{
    s32 i, j;

    Inline_UnkMatrixCopy(in, out, 0x00, 0x00);
    Inline_UnkMatrixCopy(in, out, 0x10, 0x02);
    Inline_UnkMatrixCopy(in, out, 0x20, 0x20);
    Inline_UnkMatrixCopy(in, out, 0x30, 0x22);
}

// match
void Unref_ToggleBackbuffer(void)
{
    if ((REG_DISPCNT32 & BACKBUFFER) != 0)
    {
        PTR_BACKBUFFER_UNUSED += 0xa000;
        REG_DISPCNT32 &= ~BACKBUFFER;
    }
    else
    {
        PTR_BACKBUFFER_UNUSED -= 0xa000;
        REG_DISPCNT32 |= BACKBUFFER;
    }
}

// match
void DelayFrames(u32 count)
{
    u8 j;
    u32 i;
    u32 dummy = 42069; // can be anything
    for (i = 0; i < count; i++)
    {
        WaitForVBlank();
        // Not only is this loop useless, but armcc puts an erroneous __rt_divtest(3) call
        // despite not allocating the dummy variable or calling __rt_udiv(), which itself is
        // useless because __rt_divcheck is literally this:
        // void __rt_divtest(u32 denom) {
        //     if (denom == 0) {
        //         __rt_div0(); // raise SIGFPE
        //     }
        // }
        for (j = 0; j < 100; ++j)
        {
            dummy /= 3;
        }
    }
}
// match
void BlurOut(u32 speed)
{
    s8 iVar1;

    for (iVar1 = 0; iVar1 < 0x10; ++iVar1)
    {
        REG_MOSAIC = iVar1 | ((s16)iVar1 << 4) | ((s16)iVar1 << 8) |
                     ((s16)iVar1 << 0xc);
        DelayFrames(speed);
    }
}
// match
void BlurIn(u32 speed)
{
    s8 iVar1;

    for (iVar1 = 0xF; iVar1 >= 0; --iVar1)
    {
        REG_MOSAIC = iVar1 | ((s16)iVar1 << 4) | ((s16)iVar1 << 8) |
                     ((s16)iVar1 << 0xc);
        DelayFrames(speed);
    }
}
// match
void ConvertPaletteToRGB5(const u8 *in, u16 *out)
{
    u16 i, j;
    for (i = 0, j = 0; j < 0x300; i += 1, j += 3)
    {
        out[i] = RGB8ToRGB5(in[j], in[j + 1], in[j + 2]);
    }
}
// match
void FUN_080003dc(u16 param_1, u16 param_2, s32 param_3, const u32 *param_4, u32 *param_5)
{
    u32 iVar1;
    u8 uVar4;
    u8 uVar3;
    for (uVar4 = 0; uVar4 < 8; uVar4 += 1)
    {
        iVar1 = uVar4 + param_2 * 8;
        for (uVar3 = 0; uVar3 < 2; uVar3 += 1)
        {
            param_5[uVar3 + uVar4 * 2] = param_4[uVar3 + iVar1 * param_3 * 2 + param_1 * 2];
        }
    }
}

// This appears to be some bitmap conversion routine.
//
// NONMATCHING
// Regalloc doesn't match, and the second loop isn't even close.
//
// There is just a lot of math in the third loop, and just the order of things can cause it
// to mess up (such as it NEEDS to miss a mla optimization and likes to add a frame pointer
// depending on where the temporaries are and such)
//
// Theories:
//   - I just don't have the right order of the math
//   - The types of the integer arguments are different
//   - Some arguments were declared register
//   - There is an inline function
void ConvertBitmap(int param_1, int param_2, u16 *param_3, u32 *param_4, int param_5, int param_6,
                  const u32 *param_7, u8 param_8, u8 param_9, u8 param_10)
{
    int iVar1;
    u8 uVar3;
    u8 uVar2;
    u32 tmp;
    for (gI = 0; gI < param_2; ++gI)
    {
        for (gJ = 0; gJ < param_1; ++gJ)
        {
            tmp = gI * param_1 + gJ;
            FUN_080003dc(gJ, gI, param_1, param_7, &param_4[tmp * 16]);
        }
    }
    for (gI = 0; gI < 0x400; ++gI)
    {
        param_3[gI] = param_8;
    }
    for (gI = 0; gI < param_2; ++gI)
    {
        for (gJ = 0; gJ < param_1; ++gJ)
        {
            for (uVar3 = 0; uVar3 < param_6; ++uVar3)
            {
                // loop should be
                //   mov     r0, #0
                //   cmp     r8, #0
                //   ble     .end
                // .loop:
                //   mul     r2, r1, r4
                //   ldr     lr, =globals
                //   ldrh    r3, [lr, #80]   ; gI
                //   ldrh    lr, [lr, #84]   ; gJ
                //   mul     ip, r4, r3
                //   add     ip, ip, lr      ; (yes this absolutely should be mla)
                //   add     fp, lr, sl
                //   mla     lr, r0, r4, fp
                //   add     r3, r3, r9
                //   add     r3, r3, r2
                //   add     r3, lr, r3, lsl #5
                //   lsl     r3, r3, #1
                //   add     r0, r0, #1
                //   and     r0, r0, #0xff
                //   add     ip, ip, r6
                //   strh    ip, [r5, r3]
                //   cmp     r0, r8
                //   blt     .loop
                // .end:
                for (uVar2 = 0; uVar2 < param_5; ++uVar2)
                {
                    // This isn't even close to matching
                    iVar1 = uVar2 * param_1;
                    param_3[iVar1 + gJ + param_9 + (gI + param_10 + uVar3 * param_1) * 0x20] =
                        param_1 * gI + gJ + param_8;
                }
            }
        }
    }
}

// match
void FUN_080005fc(const u32 *param_1, u32 *param_2)
{
    for (gI = 0; gI < 4; ++gI)
    {
        for (gJ = 0; gJ < 4; ++gJ)
        {
            FUN_080003dc(gJ, gI, 4, param_1,
                         &param_2[gI * 0x40 + gJ * 0x10]);
        }
    }
}
// match
void FadePalette(const u8 *in, u16 *out, u8 fade)
{
    u16 i, j;
    for (i = 0, j = 0; j < 0x300; i += 1, j += 3)
    {
        out[i] = RGB8ToRGB5(in[j + 0] / fade, in[j + 1] / fade, in[j + 2] / fade);
    }
}

// match
void DisclaimerAndCopyright(void)
{
    u32 tmpDispCnt;
    REG_DISPCNT32 = MODE_4;
    ConvertPaletteToRGB5(PTR_Palette_DisclaimerAndCopyright, PTR_BG_COLORS);
    tmpDispCnt = REG_DISPCNT32;
    REG_MOSAIC = 0;
    tmpDispCnt |= BG2_ON | BACKBUFFER;
    REG_BG2CNT |= BG_MOSAIC;
    REG_DISPCNT32 = tmpDispCnt;
    CPUCopy32(ArraySize(BG_Disclaimer), PTR_BG_Disclaimer, PTR_FRONTBUFFER);
    REG_DISPCNT32 &= ~BACKBUFFER;
    DelayFrames(400);
    CPUCopy32(ArraySize(BG_Copyright), PTR_BG_Copyright, PTR_BACKBUFFER);
    REG_DISPCNT32 |= BACKBUFFER;
    DelayFrames(300);
}

// NONMATCHING
// register allocation, and the loading of 0x04000000 into r8 should be before r0
void TitleScreen(void)
{
    u8 bVar1 = 0;
    u16 uVar3 = 0;
    u8 uVar4 = 0;
    REG_BG0CNT = BG_PRIORITY(0) | BG_TILE_BASE(0) | BG_256_COLOR | BG_MOSAIC | BG_MAP_BASE(10); // 0xac0
    REG_BG1CNT = BG_PRIORITY(3) | BG_TILE_BASE(2) | BG_256_COLOR | BG_MOSAIC | BG_MAP_BASE(11); // 0xbcb
    REG_BG2CNT = BG_PRIORITY(2) | BG_TILE_BASE(2) | BG_256_COLOR | BG_MOSAIC | BG_MAP_BASE(12); // 0xcca
    REG_BG3CNT = BG_PRIORITY(1) | BG_TILE_BASE(3) | BG_256_COLOR | BG_MOSAIC | BG_MAP_BASE(13); // 0xdcd
    REG_MOSAIC = 0;
    REG_DISPCNT32 = OBJ_1D_MAP;

    Inline_ResetSprites();
    PTR_OAM[0] = 0x206e;
    PTR_OAM[1] = 0x8068;
    PTR_OAM[2] = 2 + CHAR_START;
    ConvertPaletteToRGB5(DAT_0805fc9f, PTR_BG_COLORS);
    ConvertPaletteToRGB5(DAT_0805ffa2, PTR_OBJ_COLORS);
    ConvertBitmap(0x1e, 10, PTR_MAP_10, PTR_TILE_BASES_0, 1, 1, DAT_0800bc9c, 0, 0, 0);
    ConvertBitmap(0x10, 10, PTR_MAP_11, PTR_TILE_BASES_2, 2, 1, DAT_0801579c, 0, 0, 0);
    ConvertBitmap(0x10, 10, PTR_MAP_12, PTR_TILE_BASES_2 + 0xa00, 2, 1, DAT_08012f9c, 0xa0, 0, 8);
    ConvertBitmap(0x10, 10, PTR_MAP_13, PTR_TILE_BASES_3 + 0x400, 2, 1, DAT_0801079c, 0x40,
                 0, 10);
    REG_DISPCNT32 |= OBJ_1D_MAP;
    for (gI = 0; gI < 0x10; ++gI)
    {
        PTR_OBJ_TILES[gI] = 0;
    }
    FUN_080005fc(Sprite_Pikachu, PTR_OBJ_TILES + 0x10);
    REG_DISPCNT32 |= 0x1f00;
    while (bVar1 < 0x32)
    {
        if ((((REG_KEYINPUT & KEY_START) == 0 && (bVar1 = 1)) || bVar1 >= 1) && (uVar3 & 1) == 0) // weird
        {
            ++bVar1;
            FadePalette(DAT_0805fc9f, PTR_BG_COLORS, bVar1);
        }

        uVar3 -= 3;
        if (uVar3 % 4 == 0)
        {
            uVar4 = uVar4 + 1;
        }
        if (uVar4 >= 4)
        {
            uVar4 = 0;
        }
        DelayFrames(1);
        FUN_080005fc(Sprite_Pikachu + uVar4 * 0x100, PTR_OBJ_TILES + 0x10);
        REG_BG3HOFS = uVar3;
        REG_BG2HOFS = (s32)uVar3 >> 1;
        REG_BG1HOFS = (s32)uVar3 >> 2;
    }
}

// match
void DisplayDigit(u8 index, u16 x, u16 y, u32 offset)
{
    PTR_OAM[offset + 0] = y | OBJ_256_COLOR | OBJ_TRANSLUCENT;
    PTR_OAM[offset + 1] = x | ATTR1_SIZE_16;
    PTR_OAM[offset + 2] = index * 8 + 0x22 + CHAR_START;
}

// duplicated functions with different offsets
#define TEMPLATE_PRINT_NUM(name, offset)              \
    void name(u16 x, u16 y, u16 value)                \
    {                                                 \
        u8 data[3];                                   \
        u32 tmp;                                      \
        data[2] = value % 10;                         \
        tmp = value / 100;                            \
        data[1] = (s32)(value % 100) / 10;            \
        data[0] = tmp;                                \
        DisplayDigit(data[0], x + 0, y, offset + 0);  \
        DisplayDigit(data[1], x + 16, y, offset + 4); \
        DisplayDigit(data[2], x + 32, y, offset + 8); \
    }

TEMPLATE_PRINT_NUM(PrintScore, 0xc)
TEMPLATE_PRINT_NUM(PrintStageBonus, 0x18)
TEMPLATE_PRINT_NUM(PrintTotalScore, 0x24)

// match
void DisplayScoreOverlay(void)
{
    DisplayDigit(0xb, 0, 0, 4);
    DisplayDigit(10, 0x10, 0, 8);
    PrintScore(0x20, 0, gCurrentScore);
}
// match
void ReadKeyInputs(void)
{
    gPlayerIsMoving = false;
    if ((REG_KEYINPUT & KEY_LEFT) == 0)
    {
        if (gPlayerX >= 6)
        {
            gPlayerX = gPlayerX - 6;
        }
        gPlayerDirection = PLAYER_DIRECTION_LEFT;
        gPlayerIsMoving = 1;
    }
    if ((REG_KEYINPUT & KEY_RIGHT) == 0)
    {
        if (gPlayerX < 200)
        {
            gPlayerX = gPlayerX + 6;
        }
        gPlayerDirection = PLAYER_DIRECTION_RIGHT;
        gPlayerIsMoving = 1;
    }
    gPlayerSpriteFrame = gPlayerSpriteFrame + 1;
    if (0x17 < gPlayerSpriteFrame)
    {
        gPlayerSpriteFrame = 0;
    }
    if (gPlayerIsMoving != gPlayerWasMoving)
    {
        gPlayerSpriteFrame = 0;
    }
}

// match
void DisplayBall(u8 ball, u16 index)

{
    if (gBallState_status[ball] == 1)
    {
        PTR_OAM[index + 0] = gBallState_y[ball] | OBJ_256_COLOR;
        PTR_OAM[index + 1] = gBallState_x[ball] | ATTR1_SIZE_16;
        PTR_OAM[index + 2] =
            gBallState_rotation[ball] * 8 + gBallState_type[ball] * 0x20 + 0xc2 + CHAR_START;
    }
    if (gBallState_status[ball] == 0)
    {
        PTR_OAM[index + 0] = OBJ_Y(0xC8) | OBJ_256_COLOR;
        PTR_OAM[index + 1] = 0;
        PTR_OAM[index + 2] = 0;
    }
    if (gBallState_status[ball] > 1)
    {
        DisplayDigit(BallScore(gBallState_type[ball]), gBallState_x[ball],
                     gBallState_y[ball], index);
    }
}

// match
void SpawnBall(u8 ball)
{
    if (gRemainingPoints > 0)
    {
        // Timer based RNG
        gBallState_y[ball] = *PTR_TIMER_RNG % 16;
        gBallState_x[ball] = *PTR_TIMER_RNG % SCREEN_WIDTH;
        gBallState_speed[ball] = *PTR_TIMER_RNG % 3 + gCurrentStage;
        gBallState_type[ball] = POKE_BALL;
        // Minor bug: a Great Ball can spawn if there are only two points remaining.
        // Since the remaining points is a signed integer, it just means that the player
        // can technically get 1 extra point than they would be able to normally.
#ifdef BUGFIX
        if (*PTR_TIMER_RNG % 5 == 0 && gRemainingPoints >= BallScore(GREAT_BALL))
#else
        if (*PTR_TIMER_RNG % 5 == 0 && gRemainingPoints >= BallScore(GREAT_BALL) - 1)
#endif
        {
            gBallState_type[ball] = GREAT_BALL;
        }
        if (*PTR_TIMER_RNG % 11 == 0 && gRemainingPoints >= BallScore(ULTRA_BALL))
        {
            gBallState_type[ball] = ULTRA_BALL;
        }
        gBallState_rotation[ball] = 0;
        gBallState_status[ball] = 1;
        gRemainingPoints -= BallScore(gBallState_type[ball]);
        gBallCount += 1;
    }
}

// match
void MoveBall(u8 ball)
{
    if (gBallState_status[ball] == BALL_STATUS_NORMAL)
    {
        gBallState_y[ball] += gBallState_speed[ball];
    }
    if (gBallState_y[ball] >= 144 && gBallState_status[ball] == 1)
    {
        gBallState_status[ball] = BALL_STATUS_NONE;
        gBallCount -= 1;
        SpawnBall(ball);
    }
    gBallState_rotation[ball] += 1;
    if (gBallState_rotation[ball] > 3)
    {
        gBallState_rotation[ball] = 0;
    }
    // s
    if (gBallState_status[ball] > BALL_STATUS_NORMAL)
    {
        gBallState_y[ball] -= 4;
        gBallState_status[ball] -= 1;
    }
    if (gBallState_status[ball] == 2)
    {
        gBallState_status[ball] = BALL_STATUS_NONE;
        gBallCount -= 1;
        SpawnBall(ball);
    }
}
// match
void CheckBallCollision(u8 ball)
{
#ifdef BUGFIX // nitpick, it is a few pixels off
    if (gBallState_y[ball] < DEFAULT_PLAYER_Y)
#else
    if (gBallState_y[ball] < DEFAULT_PLAYER_X)
#endif
    {
        return;
    }
    if (gBallState_status[ball] != 1)
    {
        return;
    }
    if (gBallState_x[ball] < (gPlayerX - 0x10))
    {
        return;
    }
    if (gBallState_x[ball] > gPlayerX + 0x20)
    {
        return;
    }
    gCurrentScore += BallScore(gBallState_type[ball]);
    gBallState_status[ball] = 10;
}
static inline u32 Inline_AddStageBonus(void)
{
    return gCurrentScore + gCurrentStage * 50;
}

static inline void Inline_SetMode(void)
{
    REG_MOSAIC = 0;
    REG_BG2CNT |= BG_MOSAIC;
    REG_DISPCNT32 = MODE_4 | BACKBUFFER;
}
static inline void Inline_SetMode2(void)
{
    REG_DISPCNT32 = (REG_DISPCNT32 & ~BACKBUFFER) | OBJ_1D_MAP;
    DelayFrames(1);
}
static inline void SetDispCntFlag(u32 value)
{
    REG_DISPCNT32 = value;
}
static inline void SetStage(u8 stage)
{
    gCurrentStage = stage;
}
// match
void LevelClear(void)
{
    DelayFrames(20);
    BlurOut(1);
    Inline_ResetSprites();
    Inline_SetMode();
    CPUCopy32(ArraySize(BG_LevelClear), BG_LevelClear, PTR_FRONTBUFFER);
    ConvertPaletteToRGB5(Palette_LevelClear, PTR_BG_COLORS);
    PrintScore(0xb8, 0x3c, gCurrentScore);
    PrintStageBonus(0xb8, 0x52, gCurrentStage * 50);
    gCurrentScore = Inline_AddStageBonus();
    PrintTotalScore(0xb8, 0x6a, gCurrentScore);
    Inline_SetMode2();
    SetDispCntFlag(REG_DISPCNT32 | BG2_ENABLE | OBJ_ENABLE);
    SetStage(gCurrentStage + 1);
    DelayFrames(400);
}

// NONMATCHING: regalloc, not sure how to fix this
void GameOver(void)
{
    u8 tmp;
    DelayFrames(20);
    BlurOut(1);
    Inline_ResetSprites();
    Inline_SetMode();
    tmp = 4;
    CPUCopy32(ArraySize(BG_GameOver), BG_GameOver, PTR_FRONTBUFFER);
    ConvertPaletteToRGB5(Palette_GameOver, PTR_BG_COLORS);
    Inline_SetMode2();
    SetDispCntFlag(REG_DISPCNT32 | BG2_ENABLE);
    DelayFrames(400);
    gCurrentStage = tmp;
}

#define Inline_ChangePalette(palette)                       \
    do                                                      \
    {                                                       \
        for (gI = 1; gI < 0x20; ++gI)                       \
        {                                                   \
            FadePalette(palette, PTR_BG_COLORS, 0x20 - gI); \
            DelayFrames(1);                                 \
        }                                                   \
    } while (0)

// NONMATCHING: regalloc
void GameLoop(void)
{
    for (;;)
    {
        gCurrentScore = 0;
        gRemainingPoints = MAXIMUM_POINTS;
        gBallCount = 0;
        gPlayerX = DEFAULT_PLAYER_X;
        gPlayerY = DEFAULT_PLAYER_Y;
        if (gCurrentStage == 4)
        {
            return;
        }
        Inline_ResetSprites();
        // reset player's position
        PTR_OAM[0] = OBJ_Y(110) | OBJ_256_COLOR;
        PTR_OAM[1] = OBJ_X(104) | ATTR1_SIZE_32;
        PTR_OAM[2] = OBJ_CHAR(CHAR_START + 2);
        REG_BLDCNT = 0x440;
        REG_BLDALPHA = 0x808;
        for (gI = 0; gI < NUM_BALLS; ++gI)
        {
            DelayFrames(1);
            SpawnBall(gI);
        }
        Inline_SetMode();
        CPUCopy32(ArraySize(BG_LevelName), BG_LevelName, PTR_FRONTBUFFER);
        ConvertPaletteToRGB5(Palette_LevelName, PTR_BG_COLORS);
        PTR_OAM[4] = OBJ_Y(64) | OBJ_256_COLOR;
        PTR_OAM[5] = OBJ_X(112) | ATTR1_SIZE_16;
        PTR_OAM[6] = gCurrentStage * 8 + 0x22 + CHAR_START;
        REG_DISPCNT32 &= ~BACKBUFFER;

        FUN_080005fc(Sprite_Pikachu + 1024, PTR_OBJ_TILES + 0x10);
        REG_DISPCNT32 |= OBJ_1D_MAP;
        for (gI = 0; gI < 20; ++gI)
        {
            FUN_08000198(DAT_0805039c + gI * 0x40,
                         PTR_OBJ_TILES + gI * 0x40 + 0x110);
        }
        for (gI = 0; gI < 12; ++gI)
        {
            FUN_08000198(DAT_0805179c + gI * 0x40,
                         PTR_OBJ_TILES + gI * 0x40 + 0x610);
        }
        DelayFrames(1);
        REG_DISPCNT32 |= BG2_ENABLE | OBJ_ENABLE;
        DelayFrames(100);
        for (gI = 1; gI < 0x20; ++gI)
        {
            FadePalette(Palette_LevelName, PTR_BG_COLORS, gI);
            DelayFrames(1);
        }
        if (gCurrentStage == 1)
        {
            CPUCopy32(ArraySize(BG_Stage1), BG_Stage1, PTR_FRONTBUFFER);
            Inline_ChangePalette(Palette_Stage1);
        }
        if (gCurrentStage == 2)
        {
            CPUCopy32(ArraySize(BG_Stage2), BG_Stage2, PTR_FRONTBUFFER);
            Inline_ChangePalette(Palette_Stage2);
        }
        if (gCurrentStage == 3)
        {
            CPUCopy32(ArraySize(BG_Stage3), BG_Stage3, PTR_FRONTBUFFER);
            Inline_ChangePalette(Palette_Stage3);
        }
        for (;;)
        {
            gFrameCycle += 1;
            if (gFrameCycle > 31)
            {
                gFrameCycle = 0;
            }
            if (gFrameCycle % FRAME_SKIP == 0)
            {
                ReadKeyInputs();
                if (gFrameCycle % FRAME_SKIP == 0)
                {
                    for (gI = 0; gI < NUM_BALLS; ++gI)
                    {
                        MoveBall(gI);
                    }
                }
            }
            if (gPlayerIsMoving == false)
            {
                if (gFrameCycle % FRAME_SKIP == 0)
                {
                    for (gI = 0; gI < NUM_BALLS; ++gI)
                    {
                        CheckBallCollision(gI);
                    }
                }
            }
            if (gPlayerIsMoving == true)
            {
                gPlayerSpriteIndex = gPlayerSpriteIndexesMoving[gPlayerSpriteFrame];
            }
            else
            {
                gPlayerSpriteIndex = gPlayerSpriteIndexesStill[gPlayerSpriteFrame];
            }

            if (gBallCount < 1)
            {
                if (gCurrentScore >= MINIMUM_SCORE)
                {
                    LevelClear();
                }
                else
                {
                    GameOver();
                }
                break;
            }
            gPlayerWasMoving = gPlayerIsMoving;
            WaitForVBlank();
            FUN_080005fc(Sprite_Pikachu + gPlayerSpriteIndex * 0x100, PTR_OBJ_TILES + 0x10);
            PTR_OAM[0] = gPlayerY | ATTR0_COLOR_256;
            // redundantly redundant
            PTR_OAM[1] = gPlayerX | ATTR1_SIZE_32;
            PTR_OAM[1] = (gPlayerDirection == PLAYER_DIRECTION_RIGHT)
                             ? (gPlayerX | ATTR1_SIZE_32 | ATTR1_FLIP_X)
                             : (gPlayerX | ATTR1_SIZE_32);
            PTR_OAM[2] = OBJ_CHAR(CHAR_START + 2);
            for (gI = 0; gI < NUM_BALLS; ++gI)
            {
                DisplayBall(gI, gI * 4 + 0x18);
            }
            DisplayScoreOverlay();
        }
    }
}

// match
void AgbMain(void)
{
    for (;;)
    {
        REG_TM0CNT_H = TIMER_START;
        gCurrentStage = 1;
        gCurrentScore = 0;
        DisclaimerAndCopyright();
        TitleScreen();
        GameLoop();
    }
}
