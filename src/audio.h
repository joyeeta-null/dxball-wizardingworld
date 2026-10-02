#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"
#include "config.h"
#include "types.h"


typedef struct
{
    Sound brick, brickBreak, paddle;
    Sound lifeLost, gameOver;
    Sound levelStart, levelComplete, victory;
    Sound powerLife, powerSpeed, powerMulti, powerWide, powerInvincible;
    Sound skull, book, locked, ui;


    Sound powerSounds[5];

    Music music[MUSIC_THEMES];
    Music menuMusic;
} Audio;

void loadAudio(Audio *au);

void unloadAudio(Audio *au);


void updateMusic(Audio *au, int level, int gameStarted);

void stopLevelMusic(Audio *au);

#endif
