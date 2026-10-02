#ifndef LEVELS_H
#define LEVELS_H

#include "raylib.h"
#include "config.h"
#include "types.h"


int musicThemeIndex(int level);

float levelSpeedScale(int level);

const char *levelName(int level);

void makeLevel(Brick bricks[ROWS][COLS], int level);

#endif
