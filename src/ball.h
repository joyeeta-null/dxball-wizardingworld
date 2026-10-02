#ifndef BALL_H
#define BALL_H

#include "raylib.h"
#include "config.h"
#include "types.h"


void resetBall(Ball *ball, float paddleX, float paddleY, float paddleW);

void launchBall(Ball *ball);

void addMultiBall(Ball balls[], float x, float y, float baseSpeed);

void drawAetherOrb(Ball *ball);

void drawSnitchBall(Ball *ball, Assets *assets, int frame);

#endif
