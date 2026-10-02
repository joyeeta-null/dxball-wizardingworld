#ifndef BRICKS_H
#define BRICKS_H

#include "raylib.h"
#include "config.h"
#include "types.h"


int brickMaxHP(BrickType type);

void initBrick(Brick *b, BrickType type, float x, float y, float w, float h);

void clearBricks(Brick bricks[ROWS][COLS]);

int remainingBricks(Brick bricks[ROWS][COLS]);

void drawBricks(Brick bricks[ROWS][COLS], Assets *a);


void drawBrickSprite(Assets *a, BrickType type, Rectangle destination, Color tint);

void addBreakFX(BreakFX fx[], float x, float y, float w, float h, BrickType type);

void updateBreakFX(BreakFX fx[], float dt);

void drawBreakFX(BreakFX fx[], Assets *a);

#endif
