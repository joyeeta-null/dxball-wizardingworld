#include "raylib.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "powerups.h"
#include "ball.h"
#include "render.h"
#include "assets.h"
#include "bricks.h"
#include "theme.h"


void spawnPowerUp(PowerUp powers[], float x, float y, int level)
{
    if (level < 1)
        level = 1;
    if (level > TOTAL_LEVELS)
        level = TOTAL_LEVELS;


    const int dropChance = 16 + 2 * (level - 1);
    if (GetRandomValue(0, 99) >= dropChance)
        return;


    const int antilifeChance = 12 + 4 * (level - 1);

    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (!powers[i].active)
        {
            powers[i].x = x;
            powers[i].y = y;
            powers[i].speed = 2.0f;
            if (GetRandomValue(0, 99) < antilifeChance)
                powers[i].type = POWER_ANTILIFE;
            else
                powers[i].type = (PowerType)GetRandomValue(0, POWER_ANTILIFE - 1);
            powers[i].active = 1;
            return;
        }
    }
}

void resetPowerUps(PowerUp powers[])
{
    for (int i = 0; i < MAX_POWERUPS; i++)
        powers[i].active = 0;
}

void drawPowerUp(PowerUp *p, Assets *a)
{
    if (a->powerAtlas.id != 0)
    {
        int index = (int)p->type;
        int col = index % 3;
        int row = index / 3;
        float cellW = a->powerAtlas.width / 3.0f;
        float cellH = a->powerAtlas.height / 3.0f;
        Rectangle src = {col * cellW + cellW * 0.025f,
                         row * cellH + cellH * 0.025f,
                         cellW * 0.95f, cellH * 0.95f};
        Rectangle dst = {p->x - 18, p->y - 18, 36, 36};
        DrawCircle((int)p->x, (int)p->y, 19, Fade(ASTRAL_BRASS_HI, 0.14f));
        DrawTexturePro(a->powerAtlas, src, dst, (Vector2){0, 0}, 0.0f, WHITE);
        return;
    }

    Color fallback = (p->type == POWER_ANTILIFE) ? ASTRAL_DANGER : ASTRAL_TEAL;
    DrawCircle((int)p->x, (int)p->y, 12, fallback);
    DrawCircleLines((int)p->x, (int)p->y, 12, ASTRAL_BRASS_HI);
}

void applyPowerUp(PowerType type, Ball balls[], float *paddleW,
                  float normalPaddleW, int *lives, int *invincible,
                  float *speedMultiplier, float *speedTimer, float *wideTimer,
                  float *invincibleTimer, int *score,
                  Brick bricks[ROWS][COLS], BreakFX breakFX[], Sound sounds[])
{
    switch (type)
    {
    case POWER_LIFE:
        if (*lives < 5)
            (*lives)++;
        playSoundSafe(sounds[0]);
        break;

    case POWER_SPEED:
        *speedMultiplier = 1.35f;
        *speedTimer = 0.0f;
        playSoundSafe(sounds[1]);
        break;

    case POWER_MULTI:
    {

        int source = 0;
        for (int i = 0; i < MAX_BALLS; i++)
        {
            if (balls[i].active)
            {
                source = i;
                break;
            }
        }

        addMultiBall(balls, balls[source].x, balls[source].y, 4.0f);
        addMultiBall(balls, balls[source].x, balls[source].y, 4.0f);
        playSoundSafe(sounds[2]);
        break;
    }

    case POWER_WIDE:
        *paddleW = normalPaddleW * 1.65f;
        *wideTimer = 10.0f;
        playSoundSafe(sounds[3]);
        break;

    case POWER_INVINCIBLE:
        *invincible = 1;
        *invincibleTimer = 8.0f;
        playSoundSafe(sounds[4]);
        break;

    case POWER_SLOW:
        *speedMultiplier = 0.72f;
        *speedTimer = 0.0f;
        playSoundSafe(sounds[4]);
        break;

    case POWER_SCORE:
        *score += 100;
        playSoundSafe(sounds[0]);
        break;

    case POWER_BOMB:
    {
        int destroyed = 0;


        for (int r = ROWS - 1; r >= 0 && destroyed < 5; r--)
        {
            for (int c = 0; c < COLS && destroyed < 5; c++)
            {
                Brick *b = &bricks[r][c];
                if (!b->active)
                    continue;
                addBreakFX(breakFX, b->x, b->y, b->w, b->h, b->type);
                b->active = 0;
                *score += 10;
                destroyed++;
            }
        }
        playSoundSafe(sounds[2]);
        break;
    }

    case POWER_ANTILIFE:
        if (*lives > 0)
            (*lives)--;
        break;
    }
}
