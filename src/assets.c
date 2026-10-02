#include "raylib.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assets.h"


Texture2D loadTextureSafe(const char *path)
{
    Texture2D t = {0};
    if (FileExists(path))
        t = LoadTexture(path);
    return t;
}

Sound loadSoundSafe(const char *path)
{
    Sound s = {0};
    if (FileExists(path))
        s = LoadSound(path);
    return s;
}

Music loadMusicSafe(const char *path)
{
    Music m = {0};
    if (FileExists(path))
        m = LoadMusicStream(path);
    return m;
}

void playSoundSafe(Sound s)
{
    if (s.frameCount > 0)
        PlaySound(s);
}

void unloadTextureIfLoaded(Texture2D *t)
{
    if (t->id != 0)
        UnloadTexture(*t);
}


static void loadBrickTextures(Assets *a)
{
    a->brickAtlas = loadTextureSafe("assets/sprites/bricks.png");
    if (a->brickAtlas.id != 0)
        SetTextureFilter(a->brickAtlas, TEXTURE_FILTER_BILINEAR);
}

static void loadPlayTextures(Assets *a)
{
    a->paddleAtlas = loadTextureSafe("assets/sprites/paddles.png");
    a->powerAtlas = loadTextureSafe("assets/sprites/powerups.png");
    a->heartFull = loadTextureSafe("assets/ui/heart_full.png");
    if (a->paddleAtlas.id != 0)
        SetTextureFilter(a->paddleAtlas, TEXTURE_FILTER_BILINEAR);
    if (a->powerAtlas.id != 0)
        SetTextureFilter(a->powerAtlas, TEXTURE_FILTER_BILINEAR);
    if (a->heartFull.id != 0)
        SetTextureFilter(a->heartFull, TEXTURE_FILTER_BILINEAR);

    for (int i = 0; i < 16; i++)
    {
        a->snitch[i] = loadTextureSafe(TextFormat("assets/sprites/snitch/snitch_%02d.png", i + 1));
        if (a->snitch[i].id != 0)
            SetTextureFilter(a->snitch[i], TEXTURE_FILTER_BILINEAR);
    }

    for (int i = 0; i < TOTAL_LEVELS; i++)
    {
        a->bg[i] = loadTextureSafe(TextFormat("assets/backgrounds/level_%d.png", i + 1));
        if (a->bg[i].id != 0)
            SetTextureFilter(a->bg[i], TEXTURE_FILTER_BILINEAR);
        a->levelTitle[i] = loadTextureSafe(TextFormat("assets/ui/level_intros/level_intro_%d.png", i + 1));
        if (a->levelTitle[i].id != 0)
            SetTextureFilter(a->levelTitle[i], TEXTURE_FILTER_BILINEAR);
    }
}

static void loadUITextures(Assets *a)
{
    a->menuBackground = loadTextureSafe("assets/backgrounds/menu.png");
    a->logo = loadTextureSafe("assets/ui/logo.png");
    a->buttonPlate = loadTextureSafe("assets/ui/button.png");
    a->settingsBackground = loadTextureSafe("assets/ui/settings.png");
    a->levelSelectBackground = loadTextureSafe("assets/ui/level_select.png");
    a->highScoreBackground = loadTextureSafe("assets/ui/high_scores.png");
    a->bookPanel = loadTextureSafe("assets/ui/book_panel.png");
    a->howToPlayScreen = loadTextureSafe("assets/ui/help_controls.png");
    a->howToPlayPowerups = loadTextureSafe("assets/ui/help_powerups.png");
    a->howToPlayBricks = loadTextureSafe("assets/ui/help_bricks.png");
    if (a->menuBackground.id != 0)
        SetTextureFilter(a->menuBackground, TEXTURE_FILTER_BILINEAR);
    Texture2D *ui[] = {&a->logo, &a->buttonPlate, &a->settingsBackground,
                       &a->levelSelectBackground, &a->highScoreBackground, &a->bookPanel,
                       &a->howToPlayScreen, &a->howToPlayPowerups, &a->howToPlayBricks};
    for (int i = 0; i < (int)(sizeof(ui) / sizeof(ui[0])); i++)
        if (ui[i]->id != 0)
            SetTextureFilter(*ui[i], TEXTURE_FILTER_BILINEAR);
}

void loadAssets(Assets *a)
{
    loadBrickTextures(a);
    loadPlayTextures(a);
    loadUITextures(a);
}


static void smoothFontAtlas(Font *font)
{
    GenTextureMipmaps(&font->texture);
    SetTextureFilter(font->texture, TEXTURE_FILTER_TRILINEAR);
}

Font loadUIFont(int *loaded)
{
    *loaded = 0;

    if (FileExists("assets/fonts/Cinzel.ttf"))
    {
        Font font = LoadFontEx("assets/fonts/Cinzel.ttf", 72, NULL, 0);
        if (font.texture.id != 0)
        {
            smoothFontAtlas(&font);
            *loaded = 1;
            return font;
        }
    }

    return GetFontDefault();
}

Font loadTitleFont(int *loaded)
{
    *loaded = 0;
    if (FileExists("assets/fonts/CinzelDecorative-Regular.ttf"))
    {
        Font font = LoadFontEx("assets/fonts/CinzelDecorative-Regular.ttf", 96, NULL, 0);
        if (font.texture.id != 0)
        {
            smoothFontAtlas(&font);
            *loaded = 1;
            return font;
        }
    }
    return GetFontDefault();
}

void unloadAssets(Assets *a)
{


    Texture2D *bricks[] = {
        &a->red, &a->blue, &a->green, &a->yellow, &a->purple,
        &a->ice, &a->fire, &a->lightning, &a->rune, &a->wood,
        &a->stone, &a->stoneHit1, &a->stoneHit2,
        &a->locked, &a->lockedHit1, &a->lockedHit2,
        &a->skull, &a->book,
        &a->redBreak, &a->blueBreak, &a->greenBreak, &a->purpleBreak,
        &a->iceBreak, &a->fireBreak, &a->lightningBreak, &a->runeBreak, &a->woodBreak,
        &a->stoneBreak, &a->lockedBreak, &a->skullBreak, &a->bookBreak, &a->goldBreak};

    Texture2D *play[] = {
        &a->paddleNormal, &a->paddleWide, &a->paddleGolden, &a->paddleShield,
        &a->powerLife, &a->powerSpeed, &a->powerMulti, &a->powerWide,
        &a->powerInvincible, &a->powerAntilife, &a->heartFull};

    Texture2D *ui[] = {
        &a->levelClear, &a->gameOver, &a->youWin,
        &a->buttonPause, &a->buttonResume, &a->buttonRestart, &a->buttonMainMenu,
        &a->menuBackground, &a->menuLogo, &a->menuPlay, &a->menuHowToPlay,
        &a->menuSettings, &a->menuHighScore, &a->menuQuit,
        &a->howToPlayScreen, &a->howToPlayPowerups, &a->howToPlayBricks,
        &a->highScoreScreen, &a->panelSettings,
        &a->btnPillSmall, &a->btnPillBrown, &a->btnSquareSmall,
        &a->btnHelpNext, &a->btnHelpPrevious, &a->btnHelpBack, &a->btnSettingsBack,
        &a->btnMainMenu, &a->btnRetry, &a->btnNextLevel,
        &a->btnNextLevelNavy, &a->btnMainMenuNavy};

    unloadTextureIfLoaded(&a->brickAtlas);
    unloadTextureIfLoaded(&a->paddleAtlas);
    unloadTextureIfLoaded(&a->powerAtlas);
    unloadTextureIfLoaded(&a->logo);
    unloadTextureIfLoaded(&a->buttonPlate);
    unloadTextureIfLoaded(&a->settingsBackground);
    unloadTextureIfLoaded(&a->levelSelectBackground);
    unloadTextureIfLoaded(&a->highScoreBackground);
    unloadTextureIfLoaded(&a->bookPanel);
    for (int i = 0; i < 16; i++)
        unloadTextureIfLoaded(&a->snitch[i]);

    for (int i = 0; i < (int)(sizeof(bricks) / sizeof(bricks[0])); i++)
        unloadTextureIfLoaded(bricks[i]);

    for (int i = 0; i < (int)(sizeof(play) / sizeof(play[0])); i++)
        unloadTextureIfLoaded(play[i]);

    for (int i = 0; i < (int)(sizeof(ui) / sizeof(ui[0])); i++)
        unloadTextureIfLoaded(ui[i]);

    for (int i = 0; i < TOTAL_LEVELS; i++)
    {
        unloadTextureIfLoaded(&a->bg[i]);
        unloadTextureIfLoaded(&a->levelTitle[i]);
    }
}
