#include "raylib.h"

#include <math.h>

#include "hud.h"
#include "render.h"
#include "ball.h"
#include "bricks.h"
#include "levels.h"
#include "powerups.h"
#include "theme.h"


static void drawBackground(Assets *a, int theme)
{
    if (a->bg[theme].id != 0)
        drawTextureFit(a->bg[theme], 0, 0, SCREEN_W, SCREEN_H);
    else
        ClearBackground(ASTRAL_VOID);

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(ASTRAL_VOID, 0.14f));
    DrawRectangleGradientV(0, 0, SCREEN_W, 150, Fade(ASTRAL_VOID, 0.38f), BLANK);
    DrawRectangleGradientV(0, 480, SCREEN_W, 120, BLANK, Fade(ASTRAL_VOID, 0.28f));
}

static void drawLevelIntro(Game *g, Assets *a)
{
    int index = g->level - 1;
    if (index < 0)
        index = 0;
    if (index >= TOTAL_LEVELS)
        index = TOTAL_LEVELS - 1;

    if (a->levelTitle[index].id != 0)
    {
        drawTextureFit(a->levelTitle[index], 0, 0, SCREEN_W, SCREEN_H);
        return;
    }


    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(ASTRAL_VOID, 0.34f));
    drawAstralPanel((Rectangle){155, 163, 490, 256}, 0.98f);
    if (a->logo.id != 0)
        drawCenteredTexture(a->logo, 400, 205, 220, 110);
    drawCenteredFontText(g->font, TextFormat("CHAMBER %d", g->level), 400, 270, 16, 1.8f, ASTRAL_BRASS_HI);
    drawAstralRule(400, 292, 190);
    drawCenteredFontText(g->titleFont, levelName(g->level), 400, 327, 21, 0.4f, ASTRAL_TEXT);
    drawCenteredFontText(g->font, "PRESS ENTER TO OPEN THE WAY", 400, 378, 13, 1.0f, ASTRAL_MUTED);
}


#define HUD_MARGIN 18.0f
#define HUD_LIFE_SLOTS 5
#define HUD_LIFE_STEP 27.0f
#define HUD_HEART_W 25.0f
#define HUD_HEART_H 19.0f
#define HUD_VALUE_CENTER_Y 33.5f


static float hudLivesRight(void)
{
    return SCREEN_W - HUD_MARGIN;
}

static float hudLifeSlotX(int slot)
{
    float rightmost = hudLivesRight() - HUD_HEART_W / 2.0f;
    return rightmost - (HUD_LIFE_SLOTS - 1 - slot) * HUD_LIFE_STEP;
}

static void drawLifeHeart(Texture2D heart, float cx, float cy, int filled)
{
    if (heart.id != 0)
    {
        Rectangle src = {0, 0, (float)heart.width, (float)heart.height};
        Rectangle dst = {cx - HUD_HEART_W / 2.0f, cy - HUD_HEART_H / 2.0f,
                         HUD_HEART_W, HUD_HEART_H};
        DrawTexturePro(heart, src, dst, (Vector2){0, 0}, 0.0f,
                       filled ? WHITE : Fade(WHITE, 0.16f));
        return;
    }


    DrawPoly((Vector2){cx, cy}, 4, 8.0f, 45.0f,
             filled ? ASTRAL_BRASS : Fade(ASTRAL_MUTED, 0.20f));
}

static void drawHud(Game *g, Assets *a)
{
    DrawRectangleGradientV(0, 0, SCREEN_W, 51, Fade(ASTRAL_VOID, 0.94f), Fade(ASTRAL_NAVY, 0.80f));
    DrawLine(0, 50, SCREEN_W, 50, Fade(ASTRAL_BRASS, 0.55f));

    const float labelY = 9.0f;
    drawSolidFontText(g->font, "SCORE", (Vector2){HUD_MARGIN, labelY}, 10, 1.4f, ASTRAL_MUTED);
    drawSolidFontText(g->font, TextFormat("%06d", g->score),
                      (Vector2){HUD_MARGIN, HUD_VALUE_CENTER_Y - 17.0f / 2.0f}, 17, 0.5f, ASTRAL_TEXT);
    drawCenteredFontText(g->font, TextFormat("CHAMBER %d", g->level), SCREEN_W / 2.0f,
                         HUD_PAUSE_BUTTON_Y, 16, 1.0f, ASTRAL_BRASS_HI);

    Rectangle pause = hudPauseButtonRect();
    int hot = CheckCollisionPointRec(getMouseDesignPosition(), pause);
    DrawRectangleRounded(pause, 0.30f, 8, hot ? Fade(ASTRAL_TEAL, 0.24f) : Fade(ASTRAL_PANEL_2, 0.88f));
    DrawRectangleRoundedLinesEx(pause, 0.30f, 8, 1.2f, hot ? ASTRAL_BRASS_HI : ASTRAL_BRASS);

    DrawRectangle((int)HUD_PAUSE_BUTTON_X - 4, (int)HUD_PAUSE_BUTTON_Y - 7, 3, 14, ASTRAL_TEXT);
    DrawRectangle((int)HUD_PAUSE_BUTTON_X + 1, (int)HUD_PAUSE_BUTTON_Y - 7, 3, 14, ASTRAL_TEXT);


    const char *lives = "LIVES";


    float livesWidth = MeasureTextEx(g->font, lives, 10, 1.4f).x - 1.4f;
    drawSolidFontText(g->font, lives,
                      (Vector2){hudLivesRight() - livesWidth, labelY},
                      10, 1.4f, ASTRAL_MUTED);
    for (int i = 0; i < HUD_LIFE_SLOTS; i++)
        drawLifeHeart(a->heartFull, hudLifeSlotX(i), HUD_VALUE_CENTER_Y, i < g->lives);
}

static int paddleAtlasIndex(Game *g)
{
    if (g->invincible)
        return 3;
    if (g->speedMultiplier > 1.01f)
        return 2;
    if (g->wideTimer > 0)
        return 1;
    return 0;
}

static void drawPaddle(Game *g, Assets *a)
{
    if (a->paddleAtlas.id == 0)
    {
        DrawRectangleRounded((Rectangle){g->paddleX, g->paddleY, g->paddleW, g->paddleH},
                             0.6f, 10, ASTRAL_BRASS);
        return;
    }

    int index = paddleAtlasIndex(g);
    int col = index % 2, row = index / 2;
    float cellW = a->paddleAtlas.width / 2.0f;
    float cellH = a->paddleAtlas.height / 2.0f;
    Rectangle src = {col * cellW + cellW * 0.035f,
                     row * cellH + cellH * 0.08f,
                     cellW * 0.93f,
                     cellH * 0.72f};
    Rectangle dst = {g->paddleX, g->paddleY - 8, g->paddleW, g->paddleH + 18};
    DrawTexturePro(a->paddleAtlas, src, dst, (Vector2){0, 0}, 0.0f, WHITE);
}

static void drawActiveEffects(Game *g)
{
    float x = 18;
    if (g->invincible)
    {
        Rectangle tag = {x, 565, 118, 24};
        DrawRectangleRounded(tag, 0.4f, 8, Fade(ASTRAL_PANEL, 0.86f));
        drawCenteredFontText(g->font, TextFormat("WARD %.1fs", g->invincibleTimer),
                             tag.x + tag.width / 2, tag.y + tag.height / 2, 11, 0.3f, ASTRAL_TEAL);
        x += 126;
    }
    if (g->wideTimer > 0)
    {
        Rectangle tag = {x, 565, 118, 24};
        DrawRectangleRounded(tag, 0.4f, 8, Fade(ASTRAL_PANEL, 0.86f));
        drawCenteredFontText(g->font, TextFormat("WIDE %.1fs", g->wideTimer),
                             tag.x + tag.width / 2, tag.y + tag.height / 2, 11, 0.3f, ASTRAL_BRASS_HI);
        x += 126;
    }
    if (fabsf(g->speedMultiplier - 1.0f) > 0.01f)
    {
        Rectangle tag = {x, 565, 118, 24};
        DrawRectangleRounded(tag, 0.4f, 8, Fade(ASTRAL_PANEL, 0.86f));
        const char *kind = g->speedMultiplier < 1.0f ? "TIME" : "SWIFT";
        drawCenteredFontText(g->font, TextFormat("%s %.1fs", kind, 8.0f - g->speedTimer),
                             tag.x + tag.width / 2, tag.y + tag.height / 2, 11, 0.3f,
                             g->speedMultiplier < 1.0f ? ASTRAL_TEAL : ASTRAL_BRASS_HI);
    }
}

static void drawPlayfield(Game *g, Assets *a)
{
    drawBricks(g->bricks, a);
    drawBreakFX(g->breakFX, a);

    for (int i = 0; i < MAX_POWERUPS; i++)
        if (g->powers[i].active)
            drawPowerUp(&g->powers[i], a);

    drawPaddle(g, a);
    for (int i = 0; i < MAX_BALLS; i++)
        if (g->balls[i].active)
            drawSnitchBall(&g->balls[i], a, g->snitchFrame);

    if (g->invincible)
        DrawRing((Vector2){g->paddleX + g->paddleW / 2, g->paddleY + 8},
                 g->paddleW / 2 + 7, g->paddleW / 2 + 8, 190, 350, 40, Fade(ASTRAL_TEAL, 0.78f));
    drawActiveEffects(g);
}

static void drawPauseOverlay(Game *g, Assets *a)
{
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(ASTRAL_VOID, 0.78f));


    drawAstralPanel((Rectangle){235, 131, 330, 338}, 0.98f);
    drawCenteredFontText(g->titleFont, "GAME PAUSED", SCREEN_W / 2.0f, 173, 29, 1.0f, ASTRAL_BRASS_HI);
    drawAstralRule(SCREEN_W / 2.0f, 205, 220);

    const char *labels[3] = {"RESUME", "RESTART CHAMBER", "MAIN MENU"};
    Vector2 mouse = getMouseDesignPosition();
    for (int i = 0; i < 3; i++)
    {
        Rectangle rec = pauseMenuButtonRect(i);
        drawWizardButton(a->buttonPlate, g->font, rec, labels[i], i == 1 ? 15 : 17,
                         CheckCollisionPointRec(mouse, rec));
    }
}

static void drawEndButton(Game *g, Assets *a, float cx, const char *label)
{
    Rectangle rec = {cx - END_MENU_BUTTON_W / 2.0f,
                     END_MENU_BUTTON_Y - END_MENU_BUTTON_H / 2.0f,
                     END_MENU_BUTTON_W, END_MENU_BUTTON_H};
    drawWizardButton(a->buttonPlate, g->font, rec, label, 16,
                     CheckCollisionPointRec(getMouseDesignPosition(), rec));
}

static void drawResultPanel(Game *g, const char *eyebrow, const char *title, Color titleColor)
{
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(ASTRAL_VOID, 0.72f));
    drawAstralPanel((Rectangle){125, 128, 550, 355}, 0.98f);
    drawCenteredFontText(g->font, eyebrow, SCREEN_W / 2.0f, 172, 14, 2.0f, ASTRAL_MUTED);
    drawCenteredFontText(g->font, title, SCREEN_W / 2.0f, 216, 34, 1.0f, titleColor);
    drawAstralRule(SCREEN_W / 2.0f, 252, 220);
}

static void drawLevelClear(Game *g, Assets *a)
{
    drawResultPanel(g, TextFormat("CHAMBER %d", g->level), "MISCHIEF MANAGED", ASTRAL_SUCCESS);
    if (g->levelNewBest)
        drawCenteredFontText(g->font, TextFormat("NEW BEST  %d", g->lastLevelScore), SCREEN_W / 2.0f, 292, 20, 0.7f, ASTRAL_BRASS_HI);
    else
        drawCenteredFontText(g->font, TextFormat("THIS CHAMBER  %d     BEST  %d",
                                                 g->lastLevelScore, g->levelHighScores[g->level - 1]),
                             SCREEN_W / 2.0f, 292, 16, 0.5f, ASTRAL_TEXT);
    drawCenteredFontText(g->font, "ENTER CONTINUES TO THE NEXT CHAMBER", SCREEN_W / 2.0f, 332, 12, 0.8f, ASTRAL_MUTED);
    drawEndButton(g, a, END_RETRY_BUTTON_X, "NEXT CHAMBER");
    drawEndButton(g, a, END_MENU_BUTTON_X, "MAIN MENU");
}

static void drawVictory(Game *g, Assets *a)
{
    drawResultPanel(g, "HOGWARTS IS SAFE", "WIZARDING VICTORY", ASTRAL_BRASS_HI);
    drawCenteredFontText(g->font, TextFormat("FINAL SCORE  %d", g->score), SCREEN_W / 2.0f, 292, 20, 0.7f, ASTRAL_TEXT);
    drawCenteredFontText(g->font, "EVERY CHAMBER IS OPEN", SCREEN_W / 2.0f, 332, 12, 1.0f, ASTRAL_MUTED);
    drawEndButton(g, a, SCREEN_W / 2.0f, "MAIN MENU");
}

static void drawGameOverScreen(Game *g, Assets *a)
{
    drawResultPanel(g, "THE MAGIC HAS FADED", "GAME OVER", ASTRAL_DANGER);
    drawCenteredFontText(g->font, TextFormat("SCORE  %d", g->score), SCREEN_W / 2.0f, 292, 20, 0.7f, ASTRAL_TEXT);
    drawCenteredFontText(g->font, "R TO RETRY  /  M FOR MENU", SCREEN_W / 2.0f, 332, 12, 1.0f, ASTRAL_MUTED);
    drawEndButton(g, a, END_RETRY_BUTTON_X, "RETRY");
    drawEndButton(g, a, END_MENU_BUTTON_X, "MAIN MENU");
}

void drawPlayScreens(Game *g, Assets *a)
{
    int theme = g->level - 1;
    if (theme < 0)
        theme = 0;
    if (theme >= TOTAL_LEVELS)
        theme = TOTAL_LEVELS - 1;

    drawBackground(a, theme);
    if (g->levelIntro && !g->gameWon && !g->gameOver)
        drawLevelIntro(g, a);
    else if (!g->levelIntro)
    {
        drawHud(g, a);
        drawPlayfield(g, a);
        if (g->paused)
            drawPauseOverlay(g, a);
    }

    if (g->levelComplete)
        drawLevelClear(g, a);
    if (g->gameWon)
        drawVictory(g, a);
    if (g->gameOver)
        drawGameOverScreen(g, a);
}
