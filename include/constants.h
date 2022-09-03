#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "global.h"

#ifdef BUGFIX
#  define CHAR_START 512
#else
#  define CHAR_START 0
#endif

// The maximum number of balls
#define NUM_BALLS 10
// The ball types
#define POKE_BALL 0  // 1 point
#define GREAT_BALL 1 // 3 points
#define ULTRA_BALL 2 // 5 points

// The minimum score to clear a level
#define MINIMUM_SCORE 150
// The maximum possible points in a level before balls stop spwaning
#define MAXIMUM_POINTS 250

#define DEFAULT_PLAYER_X 104
#define DEFAULT_PLAYER_Y 112

#define PLAYER_DIRECTION_LEFT false
#define PLAYER_DIRECTION_RIGHT true

// 60 / 4 = 15fps
#define FRAME_SKIP 4

// ball status
#define BALL_STATUS_NONE 0
#define BALL_STATUS_NORMAL 1
#define BALL_STATUS_COLLECTED 2
#define BALL_STATUS_COLLECTING 10

// Sadly a 2d array seems to match better.
// struct BallState
// {
//     u8 y[NUM_BALLS];
//     u8 x[NUM_BALLS];
//     u8 speed[NUM_BALLS];
//     u8 types[NUM_BALLS];
//     u8 rotation[NUM_BALLS];
//     u8 status[NUM_BALLS];
//     u8 field_6[NUM_BALLS];
// };
enum BallStateFields {
    BALL_Y,
    BALL_X,
    BALL_SPEED,
    BALL_TYPE,
    BALL_ROTATION,
    BALL_STATUS,
    BALL_FIELD_6,
    NUM_BALL_FIELDS
};

#endif // CONSTANTS_H
