#include "global.h"
#include "rodata.c"
#include "global_variables.c"

#define ArraySize(x) (sizeof(x) / sizeof((x)[0]))
// BUG: this doesn't advance the pointer correctly and ends up filling everything
// in OAM with 0x20c8, aside from the last entries. It has no effect though
#ifdef BUGFIX
#define BUGGED_OAM_INCREMENT 4
#else
#define BUGGED_OAM_INCREMENT 1
#endif

#define Inline_ResetSprites()                               \
    do                                                      \
    {                                                       \
        for (gI = 0; gI < 0x80; gI += BUGGED_OAM_INCREMENT) \
        {                                                   \
            PTR_OAM[gI + 0] = OBJ_Y(0xc8) | OBJ_256_COLOR;  \
            PTR_OAM[gI + 1] = OBJ_X(0xfa);                  \
            PTR_OAM[gI + 2] = 0;                            \
        }                                                   \
    } while (0)

#define BallScore(ball) ((ball) * 2 + 1)

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
    } while (*pVCount != 0xA0);
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
void Unref_FUN_08000278(void)
{
    if ((REG_DISPCNT32 & BACKBUFFER) != 0)
    {
        PTR_VRAM_03000000 += 0xa000;
        REG_DISPCNT32 &= ~BACKBUFFER;
    }
    else
    {
        PTR_VRAM_03000000 -= 0xa000;
        REG_DISPCNT32 |= BACKBUFFER;
    }
}

// match
void DelayFrames(u32 count)
{
    u8 bVar1;
    u32 uVar2;
    u32 dummy = 42069; // can be anything
    for (uVar2 = 0; uVar2 < count; uVar2++)
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
        for (bVar1 = 0; bVar1 < 100; ++bVar1)
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

// NONMATCHING
// There is just a lot of math in the third loop, and just the order of things can cause it
// to mess up (such as it NEEDS to miss a mla optimization and likes to add a frame pointer
// depending on where the temporaries are and such)
//
// Theories:
//   - I just don't have the right order of the math
//   - Some arguments were declared register (register param_4 makes the first loop match)
//   - There is an inline function
void FUN_0800042c(u8 param_1, u8 param_2, u16 *param_3, register u32 *param_4, u8 param_5, u8 param_6,
                  const u32 *param_7, u8 param_8, u8 param_9, u8 param_10)
{
    int iVar1;
    u8 uVar2;
    u8 uVar3;
    u32 tmp;
    s16 tmp2;
    for (gI = 0; gI < param_2; ++gI)
    {
        for (gJ = 0; gJ < param_1; ++gJ)
        {
            tmp = param_1 * gI + gJ;
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
                for (uVar2 = 0; uVar2 < param_5; ++uVar2)
                {
                    iVar1 = uVar2 * param_1;
                    param_3[iVar1 + gJ + param_9 + (gI + param_10 + uVar3 * param_1) * 0x20] =
                        param_1 * gI + gJ + param_8;
                }
            }
        }
    }
    gI = 0;
    return;
}
// {
//     u8 uVar2, uVar3;
//     // u32 tmp;
//     for (gI = 0; gI < param_2; ++gI)
//     {
//         for (gJ = 0; gJ < param_1; ++gJ)
//         {
//             FUN_080003dc(gJ, gI, param_1, param_7, &param_4[(gI * param_1 + gJ) * 16]);
//         }
//     }
//     for (gI = 0; gI < 0x400; ++gI)
//     {
//         // for some reason there is a lsl/lsr cast for a strh in some variants
//         param_3[gI] = (s32)param_8;
//     }
//     for (gI = 0; gI < param_2; ++gI)
//     {
//         for (gJ = 0; gJ < param_1; ++gJ)
//         {
//             // This loop is the primary mismatch
//             for (uVar3 = 0; uVar3 < param_6; ++uVar3)
//             {
//                 for (uVar2 = 0; uVar2 < param_5; ++uVar2)
//                 {
//                     u32 tmp = (uVar3 * param_1);

//                     param_3[uVar2 * param_1 + gJ + (tmp + gI + param_10) * 0x20 + param_9] =
//                         gJ + param_1 * gI + param_8;
//                 }
//             }
//         }
//     }
//     gI = 0;
// }

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
    return;
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
    ConvertPaletteToRGB5(pDAT_0805f99c, PTR_BG_COLORS);
    tmpDispCnt = REG_DISPCNT32;
    REG_MOSAIC = 0;
    tmpDispCnt |= BG2_ON | BACKBUFFER;
    REG_BG2CNT |= BG_MOSAIC;
    REG_DISPCNT32 = tmpDispCnt;
    CPUCopy32(ArraySize(DAT_0800269c), pDAT_0800269c, PTR_VRAM_03000004);
    REG_DISPCNT32 &= ~BACKBUFFER;
    DelayFrames(400);
    CPUCopy32(ArraySize(DAT_0805639c), pDAT_0805639c, PTR_VRAM_03000008);
    REG_DISPCNT32 |= BACKBUFFER;
    DelayFrames(300);
}

// NONMATCHING
// register allocation, and the loading of 0x04000000 into r8 should be before r0
void TitleScreen(void)
{
    u16 uVar3 = 0;
    u8 uVar4 = 0;
    u8 bVar1 = 0;
    REG_BG0CNT = BG_PRIORITY(0) | BG_TILE_BASE(0) | BG_256_COLOR | BG_MOSAIC | BG_MAP_BASE(10); // 0xac0
    REG_BG1CNT = BG_PRIORITY(3) | BG_TILE_BASE(2) | BG_256_COLOR | BG_MOSAIC | BG_MAP_BASE(11); // 0xbcb
    REG_BG2CNT = BG_PRIORITY(2) | BG_TILE_BASE(2) | BG_256_COLOR | BG_MOSAIC | BG_MAP_BASE(12); // 0xcca
    REG_BG3CNT = BG_PRIORITY(1) | BG_TILE_BASE(3) | BG_256_COLOR | BG_MOSAIC | BG_MAP_BASE(13); // 0xdcd
    REG_MOSAIC = 0;
    REG_DISPCNT32 = OBJ_1D_MAP;

    Inline_ResetSprites();
    PTR_OAM[0] = 0x206e;
    PTR_OAM[1] = 0x8068;
    PTR_OAM[2] = 2;
    ConvertPaletteToRGB5(DAT_0805fc9f, PTR_BG_COLORS);
    ConvertPaletteToRGB5(DAT_0805ffa2, PTR_OBJ_COLORS);
    FUN_0800042c(0x1e, 10, PTR_VRAM_03000034, PTR_VRAM_03000024, 1, 1, DAT_0800bc9c, 0, 0, 0);
    FUN_0800042c(0x10, 10, PTR_VRAM_03000038, PTR_VRAM_0300002c, 2, 1, DAT_0801579c, 0, 0, 0);
    FUN_0800042c(0x10, 10, PTR_VRAM_0300003c, PTR_VRAM_0300002c + 0xa00, 2, 1, DAT_08012f9c, 0xa0, 0, 8);
    FUN_0800042c(0x10, 10, PTR_VRAM_03000040, PTR_VRAM_03000030 + 0x400, 2, 1, DAT_0801079c, 0x40,
                 0, 10);
    REG_DISPCNT32 |= OBJ_1D_MAP;
    for (gI = 0; gI < 0x10; ++gI)
    {
        PTR_VRAM_0300001c[gI] = 0;
    }
    FUN_080005fc(DAT_0805239c, PTR_VRAM_0300001c + 0x10);
    REG_DISPCNT32 |= 0x1f00;
    while (bVar1 < 0x32)
    {
        if ((((REG_KEYINPUT & KEY_START) == 0 && (bVar1 = 1)) || bVar1 >= 1) && (uVar3 & 1) == 0) // weird
        {

            ++bVar1;
            FadePalette(DAT_0805fc9f, PTR_BG_COLORS, bVar1);
        }

        uVar3 -= 3;
        if ((uVar3 & 3) == 0)
        {
            uVar4 = uVar4 + 1;
        }
        if (uVar4 >= 4)
        {
            uVar4 = 0;
        }
        DelayFrames(1);
        FUN_080005fc(DAT_0805239c + uVar4 * 0x100, PTR_VRAM_0300001c + 0x10);
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
    PTR_OAM[offset + 2] = index * 8 + 0x22;
}

// duplicated functions with different offsets
#define TEMPLATE_FUNC_ABC(name, offset)               \
    void name(u16 x, u16 y, u32 value)                \
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

TEMPLATE_FUNC_ABC(FUN_08000abc, 0xc)
TEMPLATE_FUNC_ABC(FUN_08000b48, 0x18)
TEMPLATE_FUNC_ABC(FUN_08000bd4, 0x24)

// match
void DisplayScoreOverlay(void)
{
    DisplayDigit(0xb, 0, 0, 4);
    DisplayDigit(10, 0x10, 0, 8);
    FUN_08000abc(0x20, 0, gCurrentScore);
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
void FUN_08000d30(int param_1, int param_2)

{
    if (gBallState.field_5[param_1] == 1)
    {
        PTR_OAM[param_2 + 0] = gBallState.y[param_1] | 0x2000;
        PTR_OAM[param_2 + 1] = gBallState.x[param_1] | 0x4000;
        PTR_OAM[param_2 + 2] =
            gBallState.rotation[param_1] * 8 + gBallState.types[param_1] * 0x20 + 0xc2;
    }
    if (gBallState.field_5[param_1] == 0)
    {
        PTR_OAM[param_2 + 0] = OBJ_Y(0xC8) | OBJ_256_COLOR;
        PTR_OAM[param_2 + 1] = 0;
        PTR_OAM[param_2 + 2] = 0;
    }
    if (gBallState.field_5[param_1] > 1)
    {
        DisplayDigit(BallScore(gBallState.types[param_1]), gBallState.x[param_1],
                     gBallState.y[param_1], param_2);
    }
}

// match
void SpawnBall(u8 param_1)
{
    if (gRemainingPoints > 0)
    {
        // Timer based RNG
        gBallState.y[param_1] = *pTimerRNG % 16;
        gBallState.x[param_1] = *pTimerRNG % SCREEN_WIDTH;
        gBallState.speed[param_1] = *pTimerRNG % 3 + gCurrentStage;
        gBallState.types[param_1] = POKE_BALL;
        if (*pTimerRNG % 5 == 0 && gRemainingPoints > 1)
        {
            gBallState.types[param_1] = GREAT_BALL;
        }
        if (*pTimerRNG % 11 == 0 && gRemainingPoints >= 5)
        {
            gBallState.types[param_1] = ULTRA_BALL;
        }
        gBallState.rotation[param_1] = 0;
        gBallState.field_5[param_1] = 1;
        gRemainingPoints -= BallScore(gBallState.types[param_1]);
        DAT_03000078 += 1;
    }
}

// match
void FUN_08000ecc(u8 param_1)
{
    if (gBallState.field_5[param_1] == 1)
    {
        gBallState.y[param_1] += gBallState.speed[param_1];
    }
    if (gBallState.y[param_1] >= 0x90 && gBallState.field_5[param_1] == 1)
    {
        gBallState.field_5[param_1] = 0;
        DAT_03000078 -= 1;
        SpawnBall(param_1);
    }
    gBallState.rotation[param_1] += 1;
    if (gBallState.rotation[param_1] > 3)
    {
        gBallState.rotation[param_1] = 0;
    }
    if (gBallState.field_5[param_1] > 1)
    {
        gBallState.y[param_1] += 4;
        gBallState.field_5[param_1] -= 1;
    }
    if (gBallState.field_5[param_1] == 2)
    {
        gBallState.field_5[param_1] = 0;
        DAT_03000078 -= 1;
        SpawnBall(param_1);
    }
}
// match
void FUN_08000f94(int param_1)
{
    if (gBallState.y[param_1] < 0x68)
    {
        return;
    }
    if (gBallState.field_5[param_1] != 1)
    {
        return;
    }
    if (gBallState.x[param_1] < (gPlayerX - 0x10))
    {
        return;
    }
    if (gBallState.x[param_1] > gPlayerX + 0x20)
    {
        return;
    }
    gCurrentScore += BallScore(gBallState.types[param_1]);
    gBallState.field_5[param_1] = 10;
}
// NONMATCHING
void NextStage(void)
{
    DelayFrames(0x14);
    BlurOut(1);
    Inline_ResetSprites();
    REG_MOSAIC = 0;
    REG_BG2CNT |= BG_MOSAIC;
    REG_DISPCNT32 = MODE_4 | BACKBUFFER;
    CPUCopy32(ArraySize(DAT_0803d79c), DAT_0803d79c, PTR_VRAM_03000004);
    ConvertPaletteToRGB5(DAT_08060eb1, PTR_BG_COLORS);
    FUN_08000abc(0xb8, 0x3c, gCurrentScore);
    FUN_08000b48(0xb8, 0x52, gCurrentStage * 100);
    gCurrentScore += gCurrentStage * 50;
    FUN_08000bd4(0xb8, 0x6a, gCurrentScore);
    REG_DISPCNT32 = (REG_DISPCNT32 & ~BACKBUFFER) | OBJ_1D_MAP;
    DelayFrames(1);
    REG_DISPCNT32 |= 0x1400;
    gCurrentStage += 1;
    DelayFrames(400);
}
// match
void GameOver(void)
{
    u8 tmp;
    DelayFrames(0x14);
    BlurOut(1);
    Inline_ResetSprites();
    REG_MOSAIC = 0;
    // ok then
    tmp = 4;
    REG_BG2CNT |= BG_MOSAIC;
    REG_DISPCNT32 = MODE_4 | BACKBUFFER;
    CPUCopy32(ArraySize(DAT_08046d9c), DAT_08046d9c, PTR_VRAM_03000004);
    ConvertPaletteToRGB5(DAT_080611b4, PTR_BG_COLORS);
    REG_DISPCNT32 = (REG_DISPCNT32 & ~BACKBUFFER) | OBJ_1D_MAP;
    DelayFrames(1);
    REG_DISPCNT32 |= BG2_ENABLE;
    DelayFrames(400);
    gCurrentStage = tmp;
}

static inline void Inline_ChangePalette(const u8 *palette)
{
    for (gI = 1; gI < 0x20; ++gI)
    {
        FadePalette(palette, PTR_BG_COLORS, 0x20 - gI);
        DelayFrames(1);
    }
}
// NONMATCHING
void GameLoop(void)
{
    u16 uVar1;
    for (;;)
    {
        gCurrentScore = 0;
        gRemainingPoints = MAXIMUM_POINTS;
        DAT_03000078 = '\0';
        gPlayerX = 104;
        gPlayerY = 112;
        if (gCurrentStage == 4)
        {
            return;
        }
        Inline_ResetSprites();
        // reset player's position
        PTR_OAM[0] = OBJ_Y(0x6e) | OBJ_256_COLOR;
        PTR_OAM[1] = OBJ_X(0x68) | ATTR1_SIZE_32;
        PTR_OAM[2] = OBJ_CHAR(2);
        REG_BLDCNT = 0x440;
        REG_BLDALPHA = 0x808;
        for (gI = 0; gI < NUM_BALLS; ++gI)
        {
            DelayFrames(1);
            SpawnBall(gI);
        }
        REG_MOSAIC = 0;
        REG_BG2CNT |= BG_MOSAIC;
        REG_DISPCNT32 = MODE_4 | BACKBUFFER;
        CPUCopy32(ArraySize(DAT_0803419c), DAT_0803419c, PTR_VRAM_03000004);
        ConvertPaletteToRGB5(DAT_08060bae, PTR_BG_COLORS);
        PTR_OAM[4] = OBJ_Y(0x40) | OBJ_256_COLOR;
        PTR_OAM[5] = OBJ_X(0x70) | ATTR1_SIZE_32;
        PTR_OAM[6] = gCurrentStage * 8 + 0x22;
        REG_DISPCNT32 &= ~BACKBUFFER;
        FUN_080005fc(DAT_0805339c, PTR_VRAM_0300001c + 0x10);
        REG_DISPCNT32 |= OBJ_1D_MAP;
        for (gI = 0; gI < 0x14; ++gI)
        {
            FUN_08000198(DAT_0805039c + gI * 0x40,
                         PTR_VRAM_0300001c + gI * 0x40 + 0x110);
        }
        for (gI = 0; gI < 0xc; ++gI) // NUM_BALLS?
        {
            FUN_08000198(DAT_0805179c + gI * 0x40,
                         PTR_VRAM_0300001c + gI * 0x40 + 0x610);
        }
        DelayFrames(1);
        REG_DISPCNT32 = REG_DISPCNT32 | 0x1400;
        DelayFrames(100);
        for (gI = 1; gI < 0x20; ++gI)
        {
            FadePalette(DAT_08060bae, PTR_BG_COLORS, gI & 0xff);
            DelayFrames(1);
        }
        if (gCurrentStage == 1)
        {
            CPUCopy32(ArraySize(DAT_08017f9c), DAT_08017f9c, PTR_VRAM_03000004);
            Inline_ChangePalette(DAT_080602a5);
        }
        if (gCurrentStage == 2)
        {
            CPUCopy32(ArraySize(DAT_0802159c), DAT_0802159c, PTR_VRAM_03000004);
            Inline_ChangePalette(DAT_080605a8);
        }
        if (gCurrentStage == 3)
        {
            CPUCopy32(ArraySize(DAT_0802ab9c), DAT_0802ab9c, PTR_VRAM_03000004);
            Inline_ChangePalette(DAT_080608ab);
        }
        for (;;)
        {
            gFrameCycle += 1;
            if (gFrameCycle > 31)
            {
                gFrameCycle = 0;
            }
            if (gFrameCycle % FRAME_SKIP == 0 && (ReadKeyInputs(), gFrameCycle % FRAME_SKIP == 0))
            {
                for (gI = 0; gI < NUM_BALLS; ++gI)
                {
                    FUN_08000ecc(gI);
                }
            }
            if (gPlayerIsMoving == false)
            {
                if (gFrameCycle % FRAME_SKIP == 0)
                {
                    for (gI = 0; gI < NUM_BALLS; ++gI)
                    {
                        FUN_08000f94(gI & 0xff);
                    }
                    goto LAB_08001660;
                }
            LAB_08001678:
                DAT_03000098 = DAT_030000bc[gPlayerSpriteFrame];
            }
            else
            {
            LAB_08001660:
                if (gPlayerIsMoving != true)
                    goto LAB_08001678;
                DAT_03000098 = DAT_0300009c[gPlayerSpriteFrame];
            }
            if (DAT_03000078 < 1)
                break;
            gPlayerWasMoving = gPlayerIsMoving;
            WaitForVBlank();
            FUN_080005fc(DAT_0805239c + (u32)DAT_03000098 * 0x100, PTR_VRAM_0300001c + 0x10);
            PTR_OAM[0] = gPlayerY | ATTR0_COLOR_256;
            // redundant but ok
            PTR_OAM[1] = gPlayerX | ATTR1_SIZE_32;
            if (gPlayerDirection == PLAYER_DIRECTION_RIGHT)
            {
                uVar1 = gPlayerX | ATTR1_SIZE_32 | ATTR1_FLIP_X;
            }
            else
            {
                uVar1 = gPlayerX | ATTR1_SIZE_32;
            }
            PTR_OAM[1] = uVar1;
            PTR_OAM[2] = 2;
            for (gI = 0; gI < NUM_BALLS; ++gI)
            {
                FUN_08000d30(gI & 0xff, gI * 4 + 0x18);
            }
            DisplayScoreOverlay();
        }
        if (gCurrentScore < MINIMUM_SCORE)
        {
            GameOver();
        }
        else
        {
            NextStage();
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