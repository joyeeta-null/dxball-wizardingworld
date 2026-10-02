#include "raylib.h"

#include <string.h>

#include "game.h"
#include "assets.h"
#include "ball.h"
#include "bricks.h"
#include "levels.h"
#include "powerups.h"
#include "storage.h"


void gameInit(Game *g)
{
    memset(g, 0, sizeof(*g));

    g->unlockedLevel = 1;
    g->level = 1;
    g->levelIntro = 1;
    g->savedLevel = 1;
    g->savedLives = 3;

    g->soundOn = 1;
    g->difficulty = 1;
    g->ballSpeedSetting = 1.0f;
    g->startLives = 3;

    g->paddleX = 350;
    g->paddleY = 545;
    g->normalPaddleW = 100;
    g->paddleW = g->normalPaddleW;
    g->paddleH = 17;
    g->paddleSpeed = 7;

    g->speedMultiplier = 1.0f;
    g->snitchFrame = 0;
    g->snitchTimer = 0.0f;

    for (int i = 0; i < MAX_BALLS; i++)
        g->balls[i].radius = 8;

    ensureSaveDir();

    loadSettings(&g->soundOn, &g->mouseControlEnabled, &g->difficulty,
                 &g->ballSpeedSetting, &g->paddleSpeed, &g->startLives);
    SetMasterVolume(g->soundOn ? 1.0f : 0.0f);
    g->lives = g->startLives;

    makeLevel(g->bricks, g->level);
    resetBall(&g->balls[0], g->paddleX, g->paddleY, g->paddleW);
}

void resetRound(Game *g)
{
    g->paddleW = g->normalPaddleW;
    g->wideTimer = 0;
    g->invincibleTimer = 0;
    g->invincible = 0;
    g->speedMultiplier = 1.0f;
    g->speedTimer = 0.0f;
    g->ballLaunched = 0;

    for (int i = 0; i < MAX_BALLS; i++)
        g->balls[i].active = 0;
    g->balls[0].active = 1;
    g->balls[0].radius = 8;

    resetPowerUps(g->powers);
    for (int i = 0; i < MAX_BREAK_FX; i++)
        g->breakFX[i].active = 0;
}

void triggerGameOver(Game *g, Audio *au)
{


    if (g->gameOver || g->gameWon)
        return;

    g->gameOver = 1;

    int levelScore = g->score - g->levelStartScore;
    if (saveLevelHighScore(g->playerName, g->level, levelScore))
        g->levelHighScores[g->level - 1] = levelScore;

    addHighScore(g->playerName, g->score);
    clearProgress(g->playerName);
    playSoundSafe(au->gameOver);
}


static void returnToMenu(Game *g, Audio *au)
{
    g->returnToMenuRequested = 0;
    g->nextLevelRequested = 0;

    if (g->gameStarted && !g->gameOver && !g->gameWon && !g->levelComplete)
        saveProgress(g->playerName, g->level, g->score, g->lives, g->levelStartScore, g->bricks);

    stopLevelMusic(au);

    g->gameStarted = 0;
    g->paused = 0;
    g->level = 1;
    g->score = 0;
    g->levelStartScore = 0;
    g->lives = g->startLives;
    g->levelIntro = 1;
    g->levelComplete = 0;
    g->gameWon = 0;
    g->gameOver = 0;
    g->justStartedGame = 0;

    g->showHowToPlay = 0;
    g->howToPlayPage = 0;
    g->showHighScore = 0;
    g->showSettings = 0;
    g->showCredits = 0;

    g->paddleX = 350;
    resetRound(g);

    for (int i = 0; i < MAX_BREAK_FX; i++)
        g->breakFX[i].active = 0;

    makeLevel(g->bricks, g->level);
    resetBall(&g->balls[0], g->paddleX, g->paddleY, g->paddleW);
    playSoundSafe(au->ui);
}


static void beginRun(Game *g, Audio *au)
{
    int resuming = g->resumeRequested;

    if (g->startLevelRequested)
    {

        g->level = g->selectedStartLevel;
        g->score = 0;
        g->levelStartScore = 0;
        g->lives = g->startLives;
        clearProgress(g->playerName);
    }
    else if (g->restartLevelRequested)
    {

        g->score = g->levelStartScore;
        g->lives = g->startLives;
    }
    else if (resuming)
    {
        g->levelStartScore = g->savedLevelStartScore;
    }

    g->levelNewBest = 0;
    g->lastLevelScore = 0;
    g->resumeRequested = 0;
    g->restartLevelRequested = 0;
    g->startLevelRequested = 0;
    g->selectedStartLevel = 0;
    g->gameStarted = 1;
    g->paused = 0;
    g->justStartedGame = 1;
    g->levelIntro = 1;
    g->levelComplete = 0;
    g->gameWon = 0;
    g->gameOver = 0;
    g->paddleX = 350;

    resetRound(g);
    makeLevel(g->bricks, g->level);

    if (resuming)
        applyBrickState(g->bricks, g->savedBricks);

    resetBall(&g->balls[0], g->paddleX, g->paddleY, g->paddleW);
    playSoundSafe(au->levelStart);
}


static void advanceLevel(Game *g, Audio *au)
{
    g->nextLevelRequested = 0;

    int currentTheme = musicThemeIndex(g->level);
    if (au->music[currentTheme].frameCount > 0)
        StopMusicStream(au->music[currentTheme]);

    g->level++;
    g->lives = g->startLives;
    g->levelStartScore = g->score;
    g->levelNewBest = 0;

    makeLevel(g->bricks, g->level);
    resetRound(g);

    g->paddleX = (SCREEN_W - g->paddleW) / 2;
    g->levelComplete = 0;
    g->levelIntro = 1;


    g->justStartedGame = 1;
    playSoundSafe(au->levelStart);
}

void applyTransitions(Game *g, Audio *au)
{
    if (g->returnToMenuRequested)
        returnToMenu(g, au);

    if (g->resumeRequested || g->restartLevelRequested || g->startLevelRequested)
        beginRun(g, au);


    if (g->levelComplete && !g->gameOver && !g->gameWon &&
        (g->nextLevelRequested || IsKeyPressed(KEY_ENTER)))
        advanceLevel(g, au);
}
