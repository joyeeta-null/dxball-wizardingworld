#include "raylib.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "input.h"
#include "assets.h"
#include "render.h"
#include "storage.h"

static int clickedIn(Vector2 mouse, Rectangle r)
{
    return CheckCollisionPointRec(mouse, r);
}

static void handleEscape(Game *g, Audio *au)
{
    if (!IsKeyPressed(KEY_ESCAPE))
        return;

    if (g->gameStarted && !g->levelIntro && !g->levelComplete && !g->gameOver && !g->gameWon)
    {
        g->paused = !g->paused;
        playSoundSafe(au->ui);
        return;
    }
    if (g->gameStarted)
    {
        g->returnToMenuRequested = 1;
        playSoundSafe(au->ui);
        return;
    }
    if (g->showNameEntry)
    {
        g->showNameEntry = 0;
        playSoundSafe(au->ui);
    }
    else if (g->showLevelSelect)
    {
        g->showLevelSelect = 0;
        playSoundSafe(au->ui);
    }
    else if (g->showResumePrompt)
    {
        g->showResumePrompt = 0;
        playSoundSafe(au->ui);
    }
    else if (g->showHowToPlay)
    {
        g->showHowToPlay = 0;
        g->howToPlayPage = 0;
        playSoundSafe(au->ui);
    }
    else if (g->showHighScore)
    {
        g->showHighScore = 0;
        playSoundSafe(au->ui);
    }
    else if (g->showSettings)
    {
        g->showSettings = 0;
        playSoundSafe(au->ui);
    }
    else if (g->showCredits)
    {
        g->showCredits = 0;
        playSoundSafe(au->ui);
    }
    else
    {
        g->quitRequested = 1;
    }
}

static void confirmPlayerName(Game *g, Audio *au)
{
    if (g->playerName[0] == '\0')
        snprintf(g->playerName, MAX_NAME, "PLAYER");

    g->showNameEntry = 0;
    g->nameConfirmed = 1;
    g->unlockedLevel = loadUnlockedLevel(g->playerName);
    loadLevelHighScores(g->playerName, g->levelHighScores);
    if (loadProgress(g->playerName, &g->savedLevel, &g->savedScore, &g->savedLives,
                     &g->savedLevelStartScore, g->savedBricks))
    {
        if (g->savedLevel > g->unlockedLevel)
        {
            g->unlockedLevel = g->savedLevel;
            saveUnlockedLevel(g->playerName, g->unlockedLevel);
        }
        g->showResumePrompt = 1;
    }
    else
    {
        g->showLevelSelect = 1;
    }
    playSoundSafe(au->ui);
}

static void handleNameEntry(Game *g, Audio *au)
{
    if (!g->showNameEntry)
        return;
    int typed = GetCharPressed();
    while (typed > 0)
    {
        int len = (int)strlen(g->playerName);
        if (len < MAX_NAME_CHARS && typed < 128 && isalnum(typed))
        {
            g->playerName[len] = (char)toupper(typed);
            g->playerName[len + 1] = '\0';
        }
        typed = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE))
    {
        int len = (int)strlen(g->playerName);
        if (len > 0)
            g->playerName[len - 1] = '\0';
    }

    if (IsKeyPressed(KEY_ENTER))
        confirmPlayerName(g, au);
}

static void handlePauseKeys(Game *g, Audio *au)
{
    if (IsKeyPressed(KEY_P) && g->gameStarted && !g->levelIntro &&
        !g->gameWon && !g->gameOver && !g->levelComplete)
    {
        g->paused = !g->paused;
        playSoundSafe(au->ui);
    }

    if (g->paused && IsKeyPressed(KEY_R))
        g->restartLevelRequested = 1;
}


static void handleLevelClearShortcut(Game *g)
{
    if (!g->gameStarted || g->paused || g->levelIntro ||
        g->gameWon || g->gameOver || g->levelComplete)
        return;

    if (!(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) ||
        !IsKeyPressed(KEY_L))
        return;

    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            g->bricks[r][c].active = 0;
}

static void handleHudPauseClick(Game *g, Audio *au)
{
    if (!g->gameStarted || g->paused || g->levelIntro || g->gameWon || g->gameOver || g->levelComplete)
        return;
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        return;

    Rectangle rect = hudPauseButtonRect();

    if (clickedIn(getMouseDesignPosition(), rect))
    {
        g->paused = 1;
        g->clickConsumed = 1;
        playSoundSafe(au->ui);
    }
}

static void handlePauseMenuClick(Game *g, Audio *au)
{
    if (!g->paused || !IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        return;

    Vector2 mouse = getMouseDesignPosition();

    Rectangle rows[3];
    for (int i = 0; i < 3; i++)
        rows[i] = pauseMenuButtonRect(i);

    if (clickedIn(mouse, rows[0]))
    {
        g->paused = 0;
        g->clickConsumed = 1;
        playSoundSafe(au->ui);
    }
    else if (clickedIn(mouse, rows[1]))
    {
        g->restartLevelRequested = 1;
        g->clickConsumed = 1;
    }
    else if (clickedIn(mouse, rows[2]))
    {
        g->returnToMenuRequested = 1;
        g->clickConsumed = 1;
    }
}


static void requestPlay(Game *g, Audio *au)
{
    if (g->nameConfirmed)
    {
        g->showLevelSelect = 1;
    }
    else
    {

        g->playerName[0] = '\0';
        g->showNameEntry = 1;
    }
    playSoundSafe(au->ui);
}

static void handleMenuEnter(Game *g, Audio *au)
{
    if (g->gameStarted || g->showResumePrompt || g->showNameEntry || g->showLevelSelect ||
        g->showHowToPlay || g->showHighScore || g->showSettings || g->showCredits)
        return;

    if (IsKeyPressed(KEY_ENTER))
        requestPlay(g, au);
}


static void handleResumePromptClick(Game *g, Audio *au, Vector2 mouse)
{
    Rectangle yesRect = resumePromptButtonRect(0);
    Rectangle noRect = resumePromptButtonRect(1);

    if (clickedIn(mouse, yesRect))
    {
        g->level = g->savedLevel;
        g->score = g->savedScore;
        g->lives = g->savedLives;
        g->showResumePrompt = 0;
        g->resumeRequested = 1;
    }
    else if (clickedIn(mouse, noRect))
    {
        g->showResumePrompt = 0;
        g->showLevelSelect = 1;
        playSoundSafe(au->ui);
    }
}

static void handleLevelSelectClick(Game *g, Audio *au, Vector2 mouse)
{
    Rectangle backRect = bottomActionRect(0, 1);

    if (clickedIn(mouse, backRect))
    {
        g->showLevelSelect = 0;
        playSoundSafe(au->ui);
        return;
    }

    for (int i = 0; i < TOTAL_LEVELS; i++)
    {

        if (i + 1 > g->unlockedLevel)
            continue;

        if (clickedIn(mouse, levelSelectTileRect(i)))
        {
            g->selectedStartLevel = i + 1;
            g->startLevelRequested = 1;
            g->showLevelSelect = 0;
            playSoundSafe(au->ui);
            return;
        }
    }
}

static void handleHowToPlayClick(Game *g, Audio *au, Vector2 mouse)
{


    Rectangle previousRect = helpNavRect(g->howToPlayPage, HELP_NAV_PREVIOUS);
    Rectangle backRect = helpNavRect(g->howToPlayPage, HELP_NAV_BACK);
    Rectangle nextRect = helpNavRect(g->howToPlayPage, HELP_NAV_NEXT);

    if (g->howToPlayPage < 2 && clickedIn(mouse, nextRect))
    {
        g->howToPlayPage++;
        playSoundSafe(au->ui);
    }
    else if (g->howToPlayPage > 0 && clickedIn(mouse, previousRect))
    {
        g->howToPlayPage--;
        playSoundSafe(au->ui);
    }
    else if (clickedIn(mouse, backRect))
    {
        g->showHowToPlay = 0;
        g->howToPlayPage = 0;
        playSoundSafe(au->ui);
    }
}


static void applyDifficulty(Game *g, int difficulty)
{
    static const int livesFor[3] = {4, 3, 2};
    static const float speedFor[3] = {0.85f, 1.0f, 1.25f};

    g->difficulty = difficulty;
    g->startLives = livesFor[difficulty];
    g->ballSpeedSetting = speedFor[difficulty];
}


static void resetEverything(Game *g)
{
    g->soundOn = 1;
    SetMasterVolume(1.0f);
    g->mouseControlEnabled = 0;
    g->paddleSpeed = 7.0f;
    applyDifficulty(g, 1);

    clearProgress(g->playerName);
    clearHighScore();
    clearLevelHighScores(g->playerName);
    clearUnlockedLevel(g->playerName);

    memset(g->levelHighScores, 0, sizeof(g->levelHighScores));
    g->unlockedLevel = 1;
    g->boardCount = 0;
    g->lives = g->startLives;
    g->score = 0;
    g->level = 1;
    g->showResumePrompt = 0;
}

static void handleSettingsClick(Game *g, Audio *au, Vector2 mouse)
{


    Rectangle soundToggleRect = settingsToggleRect(0);
    Rectangle ballSpeedMinusRect = settingsStepRect(1, 0);
    Rectangle ballSpeedPlusRect = settingsStepRect(1, 1);
    Rectangle paddleSpeedMinusRect = settingsStepRect(2, 0);
    Rectangle paddleSpeedPlusRect = settingsStepRect(2, 1);
    Rectangle difficultyEasyRect = settingsDifficultyRect(0);
    Rectangle difficultyNormalRect = settingsDifficultyRect(1);
    Rectangle difficultyHardRect = settingsDifficultyRect(2);
    Rectangle mouseToggleRect = settingsToggleRect(4);
    Rectangle resetButtonRect = bottomActionRect(0, 2);
    Rectangle backRect = bottomActionRect(1, 2);

    if (clickedIn(mouse, soundToggleRect))
    {
        g->soundOn = !g->soundOn;
        SetMasterVolume(g->soundOn ? 1.0f : 0.0f);
        playSoundSafe(au->ui);
    }
    else if (clickedIn(mouse, ballSpeedMinusRect))
    {
        g->ballSpeedSetting -= 0.1f;
        if (g->ballSpeedSetting < 0.5f)
            g->ballSpeedSetting = 0.5f;
        playSoundSafe(au->ui);
    }
    else if (clickedIn(mouse, ballSpeedPlusRect))
    {
        g->ballSpeedSetting += 0.1f;
        if (g->ballSpeedSetting > 2.0f)
            g->ballSpeedSetting = 2.0f;
        playSoundSafe(au->ui);
    }
    else if (clickedIn(mouse, paddleSpeedMinusRect))
    {
        g->paddleSpeed -= 1.0f;
        if (g->paddleSpeed < 3.0f)
            g->paddleSpeed = 3.0f;
        playSoundSafe(au->ui);
    }
    else if (clickedIn(mouse, paddleSpeedPlusRect))
    {
        g->paddleSpeed += 1.0f;
        if (g->paddleSpeed > 14.0f)
            g->paddleSpeed = 14.0f;
        playSoundSafe(au->ui);
    }
    else if (clickedIn(mouse, difficultyEasyRect))
    {
        applyDifficulty(g, 0);
        playSoundSafe(au->ui);
    }
    else if (clickedIn(mouse, difficultyNormalRect))
    {
        applyDifficulty(g, 1);
        playSoundSafe(au->ui);
    }
    else if (clickedIn(mouse, difficultyHardRect))
    {
        applyDifficulty(g, 2);
        playSoundSafe(au->ui);
    }
    else if (clickedIn(mouse, mouseToggleRect))
    {
        g->mouseControlEnabled = !g->mouseControlEnabled;
        playSoundSafe(au->ui);
    }
    else if (clickedIn(mouse, resetButtonRect))
    {
        resetEverything(g);
        playSoundSafe(au->ui);
    }
    else if (clickedIn(mouse, backRect))
    {
        g->showSettings = 0;
        playSoundSafe(au->ui);
    }


    saveSettings(g->soundOn, g->mouseControlEnabled, g->difficulty,
                 g->ballSpeedSetting, g->paddleSpeed, g->startLives);
}

static void handleMainMenuClick(Game *g, Audio *au, Vector2 mouse)
{
    Rectangle credits = creditsButtonRect();
    if (clickedIn(mouse, credits))
    {
        g->showCredits = 1;
        playSoundSafe(au->ui);
        return;
    }

    enum
    {
        ROW_PLAY,
        ROW_HOW_TO_PLAY,
        ROW_SETTINGS,
        ROW_HIGH_SCORE,
        ROW_QUIT
    };

    int row = -1;
    for (int i = 0; i < 5; i++)
    {
        Rectangle rect = mainMenuButtonRect(i);
        if (clickedIn(mouse, rect))
        {
            row = i;
            break;
        }
    }

    switch (row)
    {
    case ROW_PLAY:
        requestPlay(g, au);
        break;

    case ROW_HOW_TO_PLAY:
        g->showHowToPlay = 1;
        g->howToPlayPage = 0;
        playSoundSafe(au->ui);
        break;

    case ROW_SETTINGS:
        g->showSettings = 1;
        playSoundSafe(au->ui);
        break;

    case ROW_HIGH_SCORE:
        g->showHighScore = 1;
        g->boardCount = loadAllPlayerScores(g->boardPlayers, MAX_PLAYERS);
        playSoundSafe(au->ui);
        break;

    case ROW_QUIT:
        g->quitRequested = 1;
        break;

    default:
        break;
    }
}

static void handleMenuClick(Game *g, Audio *au)
{
    if (g->gameStarted || !IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        return;

    Vector2 mouse = getMouseDesignPosition();

    if (g->showNameEntry)
    {

        return;
    }

    if (g->showResumePrompt)
        handleResumePromptClick(g, au, mouse);
    else if (g->showLevelSelect)
        handleLevelSelectClick(g, au, mouse);
    else if (g->showHowToPlay)
        handleHowToPlayClick(g, au, mouse);
    else if (g->showHighScore)
    {
        Rectangle backRect = bottomActionRect(0, 1);
        if (clickedIn(mouse, backRect))
        {
            g->showHighScore = 0;
            playSoundSafe(au->ui);
        }
    }
    else if (g->showSettings)
        handleSettingsClick(g, au, mouse);
    else if (g->showCredits)
    {
        Rectangle backRect = bottomActionRect(0, 1);
        if (clickedIn(mouse, backRect))
        {
            g->showCredits = 0;
            playSoundSafe(au->ui);
        }
    }
    else
        handleMainMenuClick(g, au, mouse);
}


static void handleEndScreenInput(Game *g)
{

    if (g->gameOver && IsKeyPressed(KEY_R))
        g->restartLevelRequested = 1;

    if (!g->gameStarted || !(g->levelComplete || g->gameOver || g->gameWon))
        return;

    int clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    Vector2 mouse = getMouseDesignPosition();


    float menuButtonX = g->gameWon ? SCREEN_W / 2.0f : END_MENU_BUTTON_X;

    Rectangle mainMenuRect = {
        menuButtonX - END_MENU_BUTTON_W / 2.0f,
        END_MENU_BUTTON_Y - END_MENU_BUTTON_H / 2.0f,
        END_MENU_BUTTON_W,
        END_MENU_BUTTON_H};

    if ((clicked && clickedIn(mouse, mainMenuRect)) || IsKeyPressed(KEY_M))
    {
        g->returnToMenuRequested = 1;
        return;
    }


    Rectangle leftRect = {
        END_RETRY_BUTTON_X - END_MENU_BUTTON_W / 2.0f,
        END_MENU_BUTTON_Y - END_MENU_BUTTON_H / 2.0f,
        END_MENU_BUTTON_W,
        END_MENU_BUTTON_H};

    if (!clicked || !clickedIn(mouse, leftRect))
        return;

    if (g->levelComplete)
        g->nextLevelRequested = 1;
    else if (g->gameOver)
        g->restartLevelRequested = 1;
}


void handleInput(Game *g, Audio *au)
{
    g->clickConsumed = 0;

    handleEscape(g, au);
    handleNameEntry(g, au);
    handlePauseKeys(g, au);
    handleLevelClearShortcut(g);
    handleHudPauseClick(g, au);
    handlePauseMenuClick(g, au);
    handleMenuEnter(g, au);
    handleMenuClick(g, au);
    handleEndScreenInput(g);
}
