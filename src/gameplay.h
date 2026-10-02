#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "game.h"
#include "audio.h"


void updateGameplay(Game *g, Audio *au, float dt);


void updateAnimations(Game *g, float dt);

#endif
