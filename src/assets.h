#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"
#include "config.h"
#include "types.h"


Texture2D loadTextureSafe(const char *path);

Sound loadSoundSafe(const char *path);

Music loadMusicSafe(const char *path);

void playSoundSafe(Sound s);

void unloadTextureIfLoaded(Texture2D *t);


void loadAssets(Assets *a);

void unloadAssets(Assets *a);


Font loadUIFont(int *loaded);


Font loadTitleFont(int *loaded);

#endif
