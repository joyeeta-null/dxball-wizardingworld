#include "raylib.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ball.h"
#include "render.h"
#include "theme.h"

void resetBall(Ball *ball, float paddleX, float paddleY, float paddleW)
{
    ball->x = paddleX + paddleW / 2;
    ball->y = paddleY - ball->radius - 2;
    ball->vx = 4.0f;
    ball->vy = -4.0f;
    ball->active = 1;
}

void launchBall(Ball *ball)
{
    float targetSpeed = 7.0f;
    ball->vx = (GetRandomValue(-100, 100) / 100.0f) * 3.0f;
    if (fabsf(ball->vx) < 1.0f)
        ball->vx = (ball->vx < 0 ? -1.0f : 1.0f) * 1.5f;

    float vySquared = targetSpeed * targetSpeed - ball->vx * ball->vx;
    if (vySquared < 9.0f)
        vySquared = 9.0f;
    ball->vy = -sqrtf(vySquared);
}

void addMultiBall(Ball balls[], float x, float y, float baseSpeed)
{
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (!balls[i].active)
        {
            balls[i].active = 1;
            balls[i].radius = 8;
            balls[i].x = x;
            balls[i].y = y;
            balls[i].vx = (i % 2 == 0 ? -1.0f : 1.0f) * baseSpeed;
            balls[i].vy = -baseSpeed;
            return;
        }
    }
}

void drawAetherOrb(Ball *ball)
{
    float pulse = (sinf((float)GetTime() * 7.0f) + 1.0f) * 0.5f;
    Vector2 center = {ball->x, ball->y};
    DrawCircleV(center, ball->radius + 7.0f + pulse * 2.0f, Fade(ASTRAL_TEAL, 0.08f));
    DrawCircleV(center, ball->radius + 3.0f, Fade(ASTRAL_TEAL, 0.18f));
    DrawCircleV(center, ball->radius, (Color){184, 225, 220, 255});
    DrawCircleV((Vector2){ball->x - 2.0f, ball->y - 2.0f}, ball->radius * 0.55f, ASTRAL_TEXT);
    DrawRing(center, ball->radius + 1.5f, ball->radius + 2.3f,
             -35.0f + pulse * 20.0f, 235.0f + pulse * 20.0f, 28, ASTRAL_BRASS_HI);
}

void drawSnitchBall(Ball *ball, Assets *assets, int frame)
{
    if (frame < 0 || frame >= 16)
        frame = 0;
    Texture2D texture = assets->snitch[frame];
    if (texture.id == 0)
    {
        drawAetherOrb(ball);
        return;
    }

    float pulse = (sinf((float)GetTime() * 8.0f) + 1.0f) * 0.5f;
    float maxDimension = (float)(texture.width > texture.height ? texture.width : texture.height);
    float scale = 30.0f / maxDimension;
    float drawWidth = texture.width * scale;
    float drawHeight = texture.height * scale;
    DrawCircleV((Vector2){ball->x, ball->y}, ball->radius + 8.0f + pulse * 2.0f,
                Fade(ASTRAL_BRASS_HI, 0.12f));
    DrawTexturePro(texture,
                   (Rectangle){0, 0, (float)texture.width, (float)texture.height},
                   (Rectangle){ball->x - drawWidth / 2.0f, ball->y - drawHeight / 2.0f,
                               drawWidth, drawHeight},
                   (Vector2){0, 0}, 0.0f, WHITE);
}
