#include "raylib.h"

#include <math.h>
#include <stddef.h>

#include "gameplay.h"
#include "assets.h"
#include "ball.h"
#include "bricks.h"
#include "levels.h"
#include "powerups.h"
#include "render.h"
#include "storage.h"


static void updateLevelIntro(Game *g)
{

    if (!g->justStartedGame &&
        (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
    {
        g->levelIntro = 0;
        g->ballLaunched = 0;
        g->clickConsumed = 1;
    }
}


static void updatePaddle(Game *g)
{
    if (IsKeyDown(KEY_LEFT))
        g->paddleX -= g->paddleSpeed;
    if (IsKeyDown(KEY_RIGHT))
        g->paddleX += g->paddleSpeed;


    if (g->mouseControlEnabled && GetMouseDelta().x != 0.0f)
        g->paddleX = getMouseDesignPosition().x - g->paddleW / 2;

    if (g->paddleX < 0)
        g->paddleX = 0;
    if (g->paddleX + g->paddleW > SCREEN_W)
        g->paddleX = SCREEN_W - g->paddleW;
}


static void updatePowerTimers(Game *g, float dt)
{
    if (g->wideTimer > 0)
    {
        g->wideTimer -= dt;
        if (g->wideTimer <= 0)
            g->paddleW = g->normalPaddleW;
    }

    if (g->invincibleTimer > 0)
    {
        g->invincibleTimer -= dt;
        if (g->invincibleTimer <= 0)
            g->invincible = 0;
    }

    if (fabsf(g->speedMultiplier - 1.0f) > 0.01f)
    {

        g->speedTimer += dt;
        if (g->speedTimer > 8.0f)
        {
            g->speedMultiplier = 1.0f;
            g->speedTimer = 0;
        }
    }
}


static void updateWaitingBall(Game *g)
{
    g->balls[0].x = g->paddleX + g->paddleW / 2;
    g->balls[0].y = g->paddleY - g->balls[0].radius - 2;

    if (IsKeyPressed(KEY_SPACE) ||
        (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !g->clickConsumed))
    {
        g->ballLaunched = 1;
        launchBall(&g->balls[0]);
    }
}


static void bounceOffWalls(Ball *ball, Audio *au)
{
    if (ball->x - ball->radius <= 0)
    {
        ball->x = ball->radius;
        ball->vx = fabsf(ball->vx);
        playSoundSafe(au->paddle);
    }

    if (ball->x + ball->radius >= SCREEN_W)
    {
        ball->x = SCREEN_W - ball->radius;
        ball->vx = -fabsf(ball->vx);
        playSoundSafe(au->paddle);
    }

    if (ball->y - ball->radius <= 0)
    {
        ball->y = ball->radius;
        ball->vy = fabsf(ball->vy);
        playSoundSafe(au->paddle);
    }
}

static int activeBallCount(Game *g)
{
    int count = 0;
    for (int i = 0; i < MAX_BALLS; i++)
        if (g->balls[i].active)
            count++;
    return count;
}


static void handleBallLost(Game *g, Ball *ball, Audio *au)
{
    ball->active = 0;


    if (activeBallCount(g) > 0)
        return;

    if (!g->invincible)
    {
        g->lives--;
        playSoundSafe(au->lifeLost);

        if (g->lives <= 0)
            triggerGameOver(g, au);
    }

    if (!g->gameOver)
    {
        g->balls[0].active = 1;
        g->balls[0].radius = 8;
        resetBall(&g->balls[0], g->paddleX, g->paddleY, g->paddleW);
        g->ballLaunched = 0;
    }
}

static void bounceOffPaddle(Game *g, Ball *ball, Audio *au)
{
    Rectangle paddleRect = {g->paddleX, g->paddleY, g->paddleW, g->paddleH};

    if (ball->vy <= 0 ||
        !CheckCollisionCircleRec((Vector2){ball->x, ball->y}, ball->radius, paddleRect))
        return;

    ball->y = g->paddleY - ball->radius - 1;


    float targetSpeed = 7.0f;


    float center = g->paddleX + g->paddleW / 2;
    float hit = (ball->x - center) / (g->paddleW / 2);
    if (hit < -1)
        hit = -1;
    if (hit > 1)
        hit = 1;

    float maxVX = 5.2f;
    ball->vx = hit * maxVX;
    if (fabsf(ball->vx) < 1.0f)
        ball->vx = (ball->vx < 0 ? -1.0f : 1.0f);


    float vySquared = targetSpeed * targetSpeed - ball->vx * ball->vx;
    if (vySquared < 9.0f)
        vySquared = 9.0f;

    ball->vy = -sqrtf(vySquared);

    playSoundSafe(au->paddle);
}


static void deflectOffBrick(Ball *ball, const Brick *b, float prevX, float prevY)
{
    if (prevY + ball->radius <= b->y)
    {
        ball->y = b->y - ball->radius - 1;
        ball->vy = -fabsf(ball->vy);
    }
    else if (prevY - ball->radius >= b->y + b->h)
    {
        ball->y = b->y + b->h + ball->radius + 1;
        ball->vy = fabsf(ball->vy);
    }
    else if (prevX + ball->radius <= b->x)
    {
        ball->x = b->x - ball->radius - 1;
        ball->vx = -fabsf(ball->vx);
    }
    else if (prevX - ball->radius >= b->x + b->w)
    {
        ball->x = b->x + b->w + ball->radius + 1;
        ball->vx = fabsf(ball->vx);
    }
    else
    {

        ball->vx = -ball->vx;
        ball->vy = -ball->vy;
    }
}

static void damageBrick(Game *g, Brick *b, Audio *au)
{
    b->hp--;

    playSoundSafe(au->brick);


    if (b->type == BRICK_SKULL)
        playSoundSafe(au->skull);
    if (b->type == BRICK_BOOK)
        playSoundSafe(au->book);
    if (b->type == BRICK_LOCKED)
        playSoundSafe(au->locked);

    if (b->hp > 0)
    {

        g->score += 2;
        return;
    }

    float cx = b->x + b->w / 2;
    float cy = b->y + b->h / 2;

    addBreakFX(g->breakFX, b->x, b->y, b->w, b->h, b->type);
    spawnPowerUp(g->powers, cx, cy, g->level);

    b->active = 0;
    g->score += 10;

    playSoundSafe(au->brickBreak);


    if (b->type == BRICK_SKULL)
        g->score += 15;
    if (b->type == BRICK_BOOK)
        g->score += 20;
}


static void hitBricks(Game *g, Ball *ball, float prevX, float prevY, Audio *au)
{
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            Brick *b = &g->bricks[r][c];
            if (!b->active)
                continue;

            Rectangle rect = {b->x, b->y, b->w, b->h};
            if (!CheckCollisionCircleRec((Vector2){ball->x, ball->y}, ball->radius, rect))
                continue;

            deflectOffBrick(ball, b, prevX, prevY);
            damageBrick(g, b, au);
            return;
        }
    }
}

static void updateBalls(Game *g, Audio *au)
{
    float stageSpeed = levelSpeedScale(g->level);

    for (int bi = 0; bi < MAX_BALLS; bi++)
    {
        Ball *ball = &g->balls[bi];
        if (!ball->active)
            continue;

        if (bi == 0 && !g->ballLaunched)
            continue;

        float step = g->speedMultiplier * g->ballSpeedSetting * stageSpeed;


        int substeps = (int)ceilf(step);
        if (substeps < 1)
            substeps = 1;
        float slice = step / (float)substeps;

        for (int s = 0; s < substeps && ball->active; s++)
        {
            float prevX = ball->x;
            float prevY = ball->y;

            ball->x += ball->vx * slice;
            ball->y += ball->vy * slice;

            bounceOffWalls(ball, au);

            if (ball->y - ball->radius > SCREEN_H)
            {
                handleBallLost(g, ball, au);
                break;
            }

            bounceOffPaddle(g, ball, au);
            hitBricks(g, ball, prevX, prevY, au);
        }
    }
}


static void updatePowerUps(Game *g, Audio *au)
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (!g->powers[i].active)
            continue;

        g->powers[i].y += g->powers[i].speed;

        Rectangle powerRect = {g->powers[i].x - 13, g->powers[i].y - 16, 26, 32};
        Rectangle paddleRect = {g->paddleX, g->paddleY, g->paddleW, g->paddleH};

        if (CheckCollisionRecs(powerRect, paddleRect))
        {
            applyPowerUp(g->powers[i].type, g->balls, &g->paddleW,
                         g->normalPaddleW, &g->lives, &g->invincible,
                         &g->speedMultiplier, &g->speedTimer, &g->wideTimer,
                         &g->invincibleTimer, &g->score, g->bricks,
                         g->breakFX, au->powerSounds);
            g->powers[i].active = 0;


            if (g->lives <= 0)
            {
                triggerGameOver(g, au);
                return;
            }
        }
        else if (g->powers[i].y > SCREEN_H + 30)
        {
            g->powers[i].active = 0;
        }
    }
}


static void checkLevelCleared(Game *g, Audio *au)
{
    if (remainingBricks(g->bricks) != 0)
        return;

    g->ballLaunched = 0;

    int levelScore = g->score - g->levelStartScore;
    g->lastLevelScore = levelScore;
    g->levelNewBest = saveLevelHighScore(g->playerName, g->level, levelScore);
    if (g->levelNewBest)
        g->levelHighScores[g->level - 1] = levelScore;


    if (g->level + 1 > g->unlockedLevel)
    {
        g->unlockedLevel = g->level + 1;
        saveUnlockedLevel(g->playerName, g->unlockedLevel);
    }

    if (g->level < TOTAL_LEVELS)
    {
        g->levelComplete = 1;

        saveProgress(g->playerName, g->level + 1, g->score, g->startLives, g->score, NULL);
        playSoundSafe(au->levelComplete);
    }
    else
    {

        g->gameWon = 1;
        addHighScore(g->playerName, g->score);
        clearProgress(g->playerName);
        playSoundSafe(au->victory);
    }

    for (int i = 0; i < MAX_BALLS; i++)
        g->balls[i].active = 0;
    g->balls[0].active = 1;
    g->balls[0].radius = 8;
}


void updateGameplay(Game *g, Audio *au, float dt)
{
    if (!g->gameStarted || g->paused || g->gameOver || g->gameWon || g->levelComplete)
        return;

    if (g->levelIntro)
    {
        updateLevelIntro(g);
        return;
    }

    updatePaddle(g);
    updatePowerTimers(g, dt);

    if (!g->ballLaunched)
        updateWaitingBall(g);

    updateBalls(g, au);
    if (g->gameOver)
        return;

    updatePowerUps(g, au);
    if (g->gameOver)
        return;

    checkLevelCleared(g, au);
}

void updateAnimations(Game *g, float dt)
{
    g->snitchTimer += dt;
    if (g->snitchTimer >= 0.055f)
    {
        g->snitchTimer -= 0.055f;
        g->snitchFrame = (g->snitchFrame + 1) % 16;
    }
    updateBreakFX(g->breakFX, dt);
}
