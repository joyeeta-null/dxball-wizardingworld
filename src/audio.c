#include "raylib.h"

#include "audio.h"
#include "assets.h"
#include "levels.h"


void loadAudio(Audio *au)
{
    au->brick = loadSoundSafe("assets/sounds/brick_magic_hit.wav");
    au->brickBreak = loadSoundSafe("assets/sounds/brick_break_crystal.wav");
    au->paddle = loadSoundSafe("assets/sounds/ball_soft_hit.wav");
    au->lifeLost = loadSoundSafe("assets/sounds/life_lost_gentle.wav");
    au->gameOver = loadSoundSafe("assets/sounds/game_over_soft.wav");
    au->levelStart = loadSoundSafe("assets/sounds/level_start_cozy.wav");
    au->levelComplete = loadSoundSafe("assets/sounds/level_complete_warm.wav");
    au->victory = loadSoundSafe("assets/sounds/victory_magical_finish.wav");
    au->powerLife = loadSoundSafe("assets/sounds/extra_life_warm.wav");
    au->powerSpeed = loadSoundSafe("assets/sounds/speed_magic.wav");
    au->powerMulti = loadSoundSafe("assets/sounds/multiball_arcane.wav");
    au->powerWide = loadSoundSafe("assets/sounds/wide_paddle_glow.wav");
    au->powerInvincible = loadSoundSafe("assets/sounds/invincible_aura.wav");
    au->skull = loadSoundSafe("assets/sounds/skull_dark_chime.wav");
    au->book = loadSoundSafe("assets/sounds/magic_book_open.wav");
    au->locked = loadSoundSafe("assets/sounds/locked_mystery.wav");
    au->ui = loadSoundSafe("assets/sounds/ui_soft_chime.wav");


    au->powerSounds[0] = au->powerLife;
    au->powerSounds[1] = au->powerSpeed;
    au->powerSounds[2] = au->powerMulti;
    au->powerSounds[3] = au->powerWide;
    au->powerSounds[4] = au->powerInvincible;

    au->music[0] = loadMusicSafe("assets/sounds/level1_cozy_magic_loop.wav");
    au->music[1] = loadMusicSafe("assets/sounds/level2_arcane_library_loop.wav");
    au->music[2] = loadMusicSafe("assets/sounds/level3_mysterious_castle_loop.wav");

    au->menuMusic = loadMusicSafe("assets/sounds/menu_theme.wav");
}

void unloadAudio(Audio *au)
{
    for (int i = 0; i < MUSIC_THEMES; i++)
    {
        if (au->music[i].frameCount > 0)
            UnloadMusicStream(au->music[i]);
    }

    if (au->menuMusic.frameCount > 0)
        UnloadMusicStream(au->menuMusic);

    Sound *sounds[] = {
        &au->brick, &au->brickBreak, &au->paddle, &au->lifeLost,
        &au->gameOver, &au->levelStart, &au->levelComplete, &au->victory,
        &au->powerLife, &au->powerSpeed, &au->powerMulti, &au->powerWide,
        &au->powerInvincible, &au->skull, &au->book, &au->locked, &au->ui};

    for (int i = 0; i < (int)(sizeof(sounds) / sizeof(sounds[0])); i++)
        if (sounds[i]->frameCount > 0)
            UnloadSound(*sounds[i]);
}

void updateMusic(Audio *au, int level, int gameStarted)
{
    int theme = musicThemeIndex(level);

    if (au->music[theme].frameCount > 0)
    {
        UpdateMusicStream(au->music[theme]);
        if (gameStarted && !IsMusicStreamPlaying(au->music[theme]))
            PlayMusicStream(au->music[theme]);
    }

    if (au->menuMusic.frameCount > 0)
    {
        if (!gameStarted)
        {
            UpdateMusicStream(au->menuMusic);
            if (!IsMusicStreamPlaying(au->menuMusic))
                PlayMusicStream(au->menuMusic);
        }
        else if (IsMusicStreamPlaying(au->menuMusic))
        {
            StopMusicStream(au->menuMusic);
        }
    }
}

void stopLevelMusic(Audio *au)
{
    for (int i = 0; i < MUSIC_THEMES; i++)
    {
        if (au->music[i].frameCount > 0 && IsMusicStreamPlaying(au->music[i]))
            StopMusicStream(au->music[i]);
    }
}
