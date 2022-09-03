#ifndef VARIABLES_H
#define VARIABLES_H

#include "global.h"

// Game state
extern u16 gCurrentScore;
extern u8 gCurrentStage;
extern s16 gRemainingPoints;
extern s8 gBallCount;
extern u8 gPlayerX;
extern u8 gPlayerY;
extern bool gPlayerDirection;
extern bool gPlayerIsMoving;
extern bool gPlayerWasMoving; 
extern u8 gFrameCycle;

// sigh, why is this not a struct
extern u8 gBallState[NUM_BALL_FIELDS][NUM_BALLS];
#define gBallState_y        gBallState[BALL_Y]         // y position of ball
#define gBallState_x        gBallState[BALL_X]         // x position of ball
#define gBallState_speed    gBallState[BALL_SPEED]     // movement speed
#define gBallState_type     gBallState[BALL_TYPE]
#define gBallState_rotation gBallState[BALL_ROTATION]
#define gBallState_status   gBallState[BALL_STATUS]
#define gBallState_field_6  gBallState[BALL_FIELD_6]

extern u8 gPlayerSpriteFrame;
extern u8 gPlayerSpriteIndex;

// Loop counter variables that are in global scope for
// some dumb reason
extern u16 gI, gJ;

extern u32 *PTR_BACKBUFFER_UNUSED; // referenced but unused

#endif // VARIABLES_H
