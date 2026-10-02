#ifndef POWERUPS_H
#define POWERUPS_H

#include "raylib.h"
#include "config.h"
#include "types.h"


void spawnPowerUp(PowerUp powers[], float x, float y, int level);

void resetPowerUps(PowerUp powers[]);

void drawPowerUp(PowerUp *p, Assets *a);

void applyPowerUp(PowerType type, Ball balls[], float *paddleW,
                  float normalPaddleW, int *lives, int *invincible,
                  float *speedMultiplier, float *speedTimer, float *wideTimer,
                  float *invincibleTimer, int *score,
                  Brick bricks[ROWS][COLS], BreakFX breakFX[], Sound sounds[]);

#endif
