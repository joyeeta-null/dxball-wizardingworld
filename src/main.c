#include "raylib.h"

#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "types.h"
#include "assets.h"
#include "audio.h"
#include "game.h"
#include "gameplay.h"
#include "hud.h"
#include "input.h"
#include "menus.h"
#include "render.h"
#include "storage.h"


int main(void)
{
    const char *capturePath = getenv("DXBALL_CAPTURE");
    int captureFrames = 0;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_W, WINDOW_H, "DX BALL - Wizarding World");
    if (!IsWindowReady())
        return 1;
    SetWindowMinSize(SCREEN_W / 2, SCREEN_H / 2);
    InitAudioDevice();
    SetTargetFPS(60);
    SetRandomSeed((unsigned int)time(NULL));


    RenderTexture2D screen = LoadRenderTexture(INTERNAL_W, INTERNAL_H);
    SetTextureFilter(screen.texture, TEXTURE_FILTER_BILINEAR);
    Camera2D designCamera = {0};
    designCamera.zoom = (float)INTERNAL_SCALE;

    Assets assets = {0};
    loadAssets(&assets);

    Audio audio = {0};
    loadAudio(&audio);

    Game game;
    gameInit(&game);
    game.font = loadUIFont(&game.fontLoaded);
    game.titleFont = loadTitleFont(&game.titleFontLoaded);

    const char *captureScene = getenv("DXBALL_CAPTURE_SCENE");
    if (captureScene && captureScene[0] == 'g')
    {
        game.gameStarted = 1;
        game.levelIntro = 0;
        game.ballLaunched = 1;
    }
    else if (captureScene && captureScene[0] == 'h')
    {
        game.showHowToPlay = 1;
        game.howToPlayPage = (captureScene[1] >= '0' && captureScene[1] <= '2')
                                 ? captureScene[1] - '0'
                                 : 1;
    }
    else if (captureScene && captureScene[0] == 'i')
    {
        int previewLevel = captureScene[1] - '0';
        game.gameStarted = 1;
        game.levelIntro = 1;
        game.level = (previewLevel >= 1 && previewLevel <= TOTAL_LEVELS) ? previewLevel : 1;
    }
    else if (captureScene && captureScene[0] == 's')
    {
        game.showSettings = 1;
    }
    else if (captureScene && captureScene[0] == 'l')
    {
        game.showLevelSelect = 1;
        game.unlockedLevel = TOTAL_LEVELS;
        TextCopy(game.playerName, "HARRY");
    }
    else if (captureScene && captureScene[0] == 'c')
    {
        game.showCredits = 1;
    }
    else if (captureScene && captureScene[0] == 'b')
    {
        static const char *previewNames[] = {"HARRY", "HERMIONE", "RON", "LUNA", "NEVILLE", "GINNY"};
        game.showHighScore = 1;
        game.boardCount = 6;
        TextCopy(game.playerName, "HARRY");
        for (int p = 0; p < game.boardCount; p++)
        {
            TextCopy(game.boardPlayers[p].name, previewNames[p]);
            game.boardPlayers[p].total = 0;
            for (int l = 0; l < TOTAL_LEVELS; l++)
            {
                game.boardPlayers[p].levels[l] = (8 - p) * 120 + l * 35;
                game.boardPlayers[p].total += game.boardPlayers[p].levels[l];
            }
        }
    }
    else if (captureScene && captureScene[0] == 'n')
    {
        game.showNameEntry = 1;
        TextCopy(game.playerName, "HERMIONE");
    }
    else if (captureScene && captureScene[0] == 'r')
    {
        game.showResumePrompt = 1;
        game.savedLevel = 4;
        TextCopy(game.playerName, "HARRY");
    }
    else if (captureScene && captureScene[0] == 'p')
    {
        game.gameStarted = 1;
        game.levelIntro = 0;
        game.paused = 1;
    }
    else if (captureScene && captureScene[0] == 'e')
    {
        game.gameStarted = 1;
        game.levelIntro = 0;
        game.gameOver = 1;
        game.score = 4280;
    }

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        updateMusic(&audio, game.level, game.gameStarted);

        handleInput(&game, &audio);
        if (game.quitRequested)
            break;

        applyTransitions(&game, &audio);
        updateGameplay(&game, &audio, dt);
        updateAnimations(&game, dt);


        game.justStartedGame = 0;

        BeginTextureMode(screen);
        ClearBackground(BLACK);
        BeginMode2D(designCamera);

        if (game.gameStarted)
            drawPlayScreens(&game, &assets);
        else
            drawMenuScreens(&game, &assets);

        EndMode2D();
        EndTextureMode();

        presentScreen(screen);


        if (capturePath && capturePath[0] != '\0' && ++captureFrames >= 4)
        {
            TakeScreenshot(capturePath);
            break;
        }
    }


    if (game.gameStarted && !game.gameOver && !game.gameWon && !game.levelComplete)
        saveProgress(game.playerName, game.level, game.score, game.lives,
                     game.levelStartScore, game.bricks);


    unloadAudio(&audio);
    unloadAssets(&assets);

    if (game.fontLoaded)
        UnloadFont(game.font);
    if (game.titleFontLoaded)
        UnloadFont(game.titleFont);

    UnloadRenderTexture(screen);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
