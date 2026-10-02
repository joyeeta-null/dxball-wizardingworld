#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "config.h"
#include "types.h"
#include "audio.h"


typedef struct
{

    int gameStarted;
    int paused;
    int showHowToPlay;
    int howToPlayPage;
    int showHighScore;
    int showSettings;
    int showCredits;
    int showNameEntry;
    int showLevelSelect;
    int showResumePrompt;


    int nameConfirmed;
    char playerName[MAX_NAME];


    int level;
    int levelIntro;
    int levelComplete;
    int gameWon;
    int gameOver;
    int justStartedGame;
    int score;
    int lives;
    int levelStartScore;
    int unlockedLevel;


    int lastLevelScore;
    int levelNewBest;


    int resumeRequested;
    int restartLevelRequested;
    int startLevelRequested;
    int selectedStartLevel;
    int returnToMenuRequested;
    int nextLevelRequested;
    int quitRequested;


    int clickConsumed;


    int savedBricks[ROWS * COLS];
    int savedLevel;
    int savedScore;
    int savedLives;
    int savedLevelStartScore;


    int levelHighScores[TOTAL_LEVELS];


    PlayerScores boardPlayers[MAX_PLAYERS];
    int boardCount;


    int soundOn;
    int mouseControlEnabled;
    int difficulty;
    float ballSpeedSetting;
    int startLives;


    float paddleX, paddleY;
    float paddleW, paddleH;
    float normalPaddleW;
    float paddleSpeed;


    float wideTimer;
    float invincibleTimer;
    float speedMultiplier;
    float speedTimer;
    int invincible;


    Ball balls[MAX_BALLS];
    int ballLaunched;
    Brick bricks[ROWS][COLS];
    PowerUp powers[MAX_POWERUPS];
    BreakFX breakFX[MAX_BREAK_FX];


    Font font;
    Font titleFont;
    int fontLoaded;
    int titleFontLoaded;
    int snitchFrame;
    float snitchTimer;
} Game;


void gameInit(Game *g);


void applyTransitions(Game *g, Audio *au);


void triggerGameOver(Game *g, Audio *au);


void resetRound(Game *g);

#endif
