#ifndef STORAGE_H
#define STORAGE_H

#include "raylib.h"
#include "config.h"
#include "types.h"


void ensureSaveDir(void);

void saveProgress(const char *playerName, int level, int score, int lives,
                  int levelStartScore, Brick bricks[ROWS][COLS]);

void clearProgress(const char *playerName);

int loadProgress(const char *playerName, int *level, int *score, int *lives,
                 int *levelStartScore, int *brickState);

void applyBrickState(Brick bricks[ROWS][COLS], const int *brickState);

int loadUnlockedLevel(const char *playerName);

void saveUnlockedLevel(const char *playerName, int level);

int loadHighScores(ScoreEntry list[]);

void addHighScore(const char *playerName, int score);

void clearHighScore(void);


int loadLevelHighScores(const char *playerName, int scores[TOTAL_LEVELS]);


int saveLevelHighScore(const char *playerName, int level, int score);

void clearUnlockedLevel(const char *playerName);

void clearLevelHighScores(const char *playerName);


int loadAllPlayerScores(PlayerScores list[], int maxPlayers);

void saveSettings(int soundOn, int mouseControl, int difficulty, float ballSpeed, float paddleSpeed, int startLives);

void loadSettings(int *soundOn, int *mouseControl, int *difficulty, float *ballSpeed, float *paddleSpeed, int *startLives);

#endif
