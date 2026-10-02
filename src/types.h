#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"
#include "config.h"


typedef enum
{
    BRICK_NONE = 0,
    BRICK_RED,
    BRICK_BLUE,
    BRICK_GREEN,
    BRICK_YELLOW,
    BRICK_PURPLE,
    BRICK_ICE,
    BRICK_FIRE,
    BRICK_LIGHTNING,
    BRICK_RUNE,
    BRICK_WOOD,
    BRICK_STONE,
    BRICK_LOCKED,
    BRICK_SKULL,
    BRICK_BOOK
} BrickType;

typedef enum
{
    POWER_LIFE,
    POWER_SPEED,
    POWER_MULTI,
    POWER_WIDE,
    POWER_INVINCIBLE,
    POWER_SLOW,
    POWER_SCORE,
    POWER_BOMB,
    POWER_ANTILIFE
} PowerType;

typedef struct
{
    float x, y;
    float vx, vy;
    float radius;
    int active;
} Ball;

typedef struct
{
    float x, y;
    PowerType type;
    float speed;
    int active;
} PowerUp;

typedef struct
{
    float x, y;
    float w, h;
    BrickType type;
    int hp;
    int active;
} Brick;

typedef struct
{
    float x, y, w, h;
    BrickType type;
    float timer;
    int active;
} BreakFX;

typedef struct
{
    char name[MAX_NAME];
    int score;
} ScoreEntry;


typedef struct
{
    char name[MAX_NAME];
    int levels[TOTAL_LEVELS];
    int total;
} PlayerScores;

typedef struct
{


    Texture2D brickAtlas, paddleAtlas, powerAtlas;
    Texture2D logo, buttonPlate, settingsBackground, levelSelectBackground;
    Texture2D highScoreBackground, bookPanel;
    Texture2D snitch[16];

    Texture2D red, blue, green, yellow, purple;
    Texture2D ice, fire, lightning, rune, wood;
    Texture2D stone, stoneHit1, stoneHit2;
    Texture2D locked, lockedHit1, lockedHit2;
    Texture2D skull, book;

    Texture2D redBreak, blueBreak, greenBreak, purpleBreak;
    Texture2D iceBreak, fireBreak, lightningBreak, runeBreak, woodBreak;
    Texture2D stoneBreak, lockedBreak, skullBreak, bookBreak, goldBreak;

    Texture2D paddleNormal, paddleWide, paddleGolden, paddleShield;

    Texture2D powerLife, powerSpeed, powerMulti, powerWide, powerInvincible, powerAntilife;

    Texture2D bg[TOTAL_LEVELS];
    Texture2D levelTitle[TOTAL_LEVELS];
    Texture2D levelClear, gameOver, youWin;
    Texture2D buttonPause, buttonResume;
    Texture2D buttonRestart, buttonMainMenu;
    Texture2D menuBackground;
    Texture2D menuLogo;
    Texture2D menuPlay;
    Texture2D menuHowToPlay;
    Texture2D menuSettings;
    Texture2D howToPlayScreen;
    Texture2D howToPlayPowerups;
    Texture2D howToPlayBricks;
    Texture2D highScoreScreen;
    Texture2D menuHighScore;
    Texture2D menuQuit;
    Texture2D heartFull;
    Texture2D panelSettings, btnPillSmall, btnPillBrown, btnSquareSmall;
    Texture2D btnNextLevelNavy, btnMainMenuNavy;
    Texture2D btnHelpNext, btnHelpPrevious, btnHelpBack, btnSettingsBack;
    Texture2D btnMainMenu, btnRetry, btnNextLevel;
} Assets;

#endif
