#include "raylib.h"

#include <math.h>
#include <string.h>

#include "menus.h"
#include "render.h"
#include "levels.h"
#include "theme.h"


static void drawMenuBackdrop(Assets *a, float darkness)
{
    if (a->menuBackground.id != 0)
        drawTextureFit(a->menuBackground, 0, 0, SCREEN_W, SCREEN_H);
    else
        ClearBackground(ASTRAL_VOID);

    DrawRectangleGradientV(0, 0, SCREEN_W, SCREEN_H,
                           Fade(ASTRAL_VOID, darkness * 0.55f),
                           Fade(ASTRAL_VOID, darkness));
    DrawRectangleGradientH(0, 0, 120, SCREEN_H, Fade(ASTRAL_VOID, 0.38f), BLANK);
    DrawRectangleGradientH(SCREEN_W - 120, 0, 120, SCREEN_H, BLANK, Fade(ASTRAL_VOID, 0.38f));
}

static void drawGeneratedBackdrop(Texture2D texture, Assets *a, float shade)
{
    if (texture.id != 0)
        drawTextureFit(texture, 0, 0, SCREEN_W, SCREEN_H);
    else
        drawMenuBackdrop(a, shade);
    if (shade > 0.0f)
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(ASTRAL_VOID, shade));
}

static void drawBackButton(Game *g, Assets *a, Rectangle rec, const char *label)
{
    int hot = CheckCollisionPointRec(getMouseDesignPosition(), rec);
    drawWizardButton(a->buttonPlate, g->font, rec, label, 16, hot);
}


static void drawHowToPlay(Game *g, Assets *a)
{
    Texture2D pages[3] = {a->howToPlayScreen, a->howToPlayPowerups, a->howToPlayBricks};
    int page = g->howToPlayPage;
    if (page < 0)
        page = 0;
    if (page > 2)
        page = 2;
    drawGeneratedBackdrop(pages[page].id != 0 ? pages[page] : a->bookPanel, a, 0.0f);

    Vector2 mouse = getMouseDesignPosition();
    Rectangle previous = helpNavRect(page, HELP_NAV_PREVIOUS);
    Rectangle back = helpNavRect(page, HELP_NAV_BACK);
    Rectangle next = helpNavRect(page, HELP_NAV_NEXT);
    if (previous.width > 0.0f)
        drawWizardButton(a->buttonPlate, g->font, previous, "PREVIOUS", 15, CheckCollisionPointRec(mouse, previous));
    drawWizardButton(a->buttonPlate, g->font, back, "BACK", 16, CheckCollisionPointRec(mouse, back));
    if (next.width > 0.0f)
        drawWizardButton(a->buttonPlate, g->font, next, "NEXT", 16, CheckCollisionPointRec(mouse, next));
}


#define LEDGER_X 116.0f
#define LEDGER_W 568.0f
#define LEDGER_RANK_X 132.0f
#define LEDGER_NAME_X 150.0f
#define LEDGER_COL_X 282.0f
#define LEDGER_COL_W 45.0f

#define LEDGER_TOTAL_RIGHT 668.0f
#define LEDGER_ROW_TOP 224.0f
#define LEDGER_ROW_STEP 25.0f
#define LEDGER_ROW_H 23.0f

static float ledgerColumnCenterX(int level)
{
    return LEDGER_COL_X + level * LEDGER_COL_W + LEDGER_COL_W / 2.0f;
}


static void drawRightFontText(Font font, const char *text, float right, float cy,
                              float fontSize, float spacing, Color color)
{
    Vector2 extent = MeasureTextEx(font, text, fontSize, spacing);
    drawSolidFontText(font, text, (Vector2){right - extent.x, cy - extent.y / 2.0f},
                      fontSize, spacing, color);
}

static void drawScoreRows(Game *g)
{
    int shown = (g->boardCount > SCOREBOARD_ROWS) ? SCOREBOARD_ROWS : g->boardCount;

    for (int i = 0; i < shown; i++)
    {
        PlayerScores *p = &g->boardPlayers[i];
        float top = LEDGER_ROW_TOP + i * LEDGER_ROW_STEP;


        float mid = top + LEDGER_ROW_H / 2.0f;
        int isMe = (g->playerName[0] != '\0' && strcmp(p->name, g->playerName) == 0);
        Color rowFill = (i % 2 == 0) ? Fade(WIZARD_BURGUNDY, 0.065f) : Fade(ASTRAL_BRASS, 0.045f);
        if (isMe)
            rowFill = Fade(ASTRAL_TEAL, 0.18f);
        DrawRectangleRounded((Rectangle){LEDGER_X, top, LEDGER_W, LEDGER_ROW_H}, 0.2f, 6, rowFill);

        Color color = (i == 0) ? WIZARD_BURGUNDY : (isMe ? ASTRAL_TEAL : WIZARD_INK);
        drawCenteredFontText(g->font, TextFormat("%d", i + 1), LEDGER_RANK_X, mid, 12, 0.2f, color);
        drawSolidFontText(g->font, p->name,
                          (Vector2){LEDGER_NAME_X, mid - MeasureTextEx(g->font, p->name, 13, 0.15f).y / 2.0f},
                          13, 0.15f, color);

        for (int l = 0; l < TOTAL_LEVELS; l++)
        {
            const char *cell = p->levels[l] > 0 ? TextFormat("%d", p->levels[l]) : "-";
            drawCenteredFontText(g->font, cell, ledgerColumnCenterX(l), mid, 11.5f, 0.1f,
                                 p->levels[l] > 0 ? color : WIZARD_INK_MUTED);
        }

        drawRightFontText(g->font, TextFormat("%d", p->total), LEDGER_TOTAL_RIGHT, mid,
                          13, 0.15f, color);
    }
}

static void drawHighScoreBoard(Game *g, Assets *a)
{
    drawGeneratedBackdrop(a->highScoreBackground, a, 0.0f);
    drawCenteredFontText(g->titleFont, "HIGH SCORES", SCREEN_W / 2.0f, 161, 27, 0.8f, WIZARD_BURGUNDY);
    drawCenteredFontText(g->font, "BEST SCORE IN EACH CHAMBER", SCREEN_W / 2.0f, 184, 11, 0.8f, WIZARD_INK_MUTED);

    const float headerY = 206.0f;
    drawCenteredFontText(g->font, "#", LEDGER_RANK_X, headerY, 11, 0.2f, WIZARD_INK_MUTED);
    drawSolidFontText(g->font, "PLAYER",
                      (Vector2){LEDGER_NAME_X, headerY - MeasureTextEx(g->font, "PLAYER", 11, 0.2f).y / 2.0f},
                      11, 0.2f, WIZARD_INK_MUTED);
    for (int l = 0; l < TOTAL_LEVELS; l++)
        drawCenteredFontText(g->font, TextFormat("L%d", l + 1), ledgerColumnCenterX(l),
                             headerY, 10.5f, 0.1f, WIZARD_INK_MUTED);

    drawRightFontText(g->font, "TOTAL", LEDGER_TOTAL_RIGHT, headerY, 11, 0.2f, WIZARD_INK_MUTED);
    DrawLineEx((Vector2){LEDGER_X, 218}, (Vector2){LEDGER_X + LEDGER_W, 218}, 1.2f,
               Fade(WIZARD_INK_MUTED, 0.42f));

    if (g->boardCount == 0)
    {

        drawCenteredFontText(g->font, "NO SCORES YET", SCREEN_W / 2.0f, 310, 20, 0.8f, WIZARD_BURGUNDY);
        drawCenteredFontText(g->font, "CLEAR A CHAMBER TO ENTER THE LEDGER", SCREEN_W / 2.0f, 340, 11, 0.5f, WIZARD_INK_MUTED);
    }
    else
        drawScoreRows(g);

    drawBackButton(g, a, bottomActionRect(0, 1), "BACK");
}


static void drawStepButton(Game *g, Assets *a, Rectangle rec, const char *label)
{
    (void)a;
    int hot = CheckCollisionPointRec(getMouseDesignPosition(), rec);
    Vector2 center = {rec.x + rec.width / 2.0f, rec.y + rec.height / 2.0f};
    float radius = rec.width / 2.0f;

    DrawCircleV((Vector2){center.x + 1.5f, center.y + 2.5f}, radius, Fade(BLACK, 0.28f));


#if RAYLIB_VERSION_MAJOR >= 6
    DrawCircleGradient((Vector2){center.x, center.y - radius * 0.22f}, radius,
#else
    DrawCircleGradient((int)center.x, (int)(center.y - radius * 0.22f), radius,
#endif
                       hot ? (Color){150, 52, 48, 255} : (Color){126, 71, 46, 255},
                       hot ? (Color){92, 24, 28, 255} : (Color){58, 29, 21, 255});
    DrawRing(center, radius - 2.4f, radius, 0, 360, 36, hot ? ASTRAL_BRASS_HI : ASTRAL_BRASS);


    float glyphOffset = (label[0] == '-') ? -0.5f : 0.0f;
    drawCenteredFontText(g->font, label, center.x, center.y + glyphOffset, 19, 0.2f,
                         hot ? (Color){255, 242, 184, 255} : ASTRAL_TEXT);
}

static void drawSettingsScreen(Game *g, Assets *a)
{
    drawGeneratedBackdrop(a->settingsBackground, a, 0.05f);


    drawCenteredFontText(g->titleFont, "WIZARDING SETTINGS", SCREEN_W / 2.0f, 111, 25, 1.0f, WIZARD_BURGUNDY);
    drawCenteredFontText(g->font, "TUNE YOUR BROOM AND SPELLS", SCREEN_W / 2.0f, 135, 11, 1.0f, WIZARD_INK_MUTED);

    static const char *labels[] = {"SOUND", "SNITCH SPEED", "BROOM SPEED", "DIFFICULTY", "MOUSE / TOUCH"};
    for (int i = 0; i < 5; i++)
    {
        const float size = 16.0f;
        Vector2 extent = MeasureTextEx(g->font, labels[i], size, 0.7f);
        drawSolidFontText(g->font, labels[i],
                          (Vector2){settingsLabelX(), settingsRowCenterY(i) - extent.y / 2.0f},
                          size, 0.7f, WIZARD_INK);
    }

    Vector2 mouse = getMouseDesignPosition();
    Rectangle sound = settingsToggleRect(0);
    Rectangle mouseToggle = settingsToggleRect(4);
    Rectangle reset = bottomActionRect(0, 2);
    Rectangle back = bottomActionRect(1, 2);
    drawWizardButton(a->buttonPlate, g->font, sound, g->soundOn ? "ON" : "OFF", 17, CheckCollisionPointRec(mouse, sound));
    drawWizardButton(a->buttonPlate, g->font, mouseToggle, g->mouseControlEnabled ? "ON" : "OFF", 17,
                     CheckCollisionPointRec(mouse, mouseToggle));
    drawWizardButton(a->buttonPlate, g->font, reset, "RESET", 16, CheckCollisionPointRec(mouse, reset));

    drawStepButton(g, a, settingsStepRect(1, 0), "-");
    drawStepButton(g, a, settingsStepRect(1, 1), "+");
    drawStepButton(g, a, settingsStepRect(2, 0), "-");
    drawStepButton(g, a, settingsStepRect(2, 1), "+");
    drawCenteredFontText(g->font, TextFormat("%.1fx", g->ballSpeedSetting),
                         settingsControlCenterX(), settingsRowCenterY(1), 18, 0.5f, WIZARD_BURGUNDY);
    drawCenteredFontText(g->font, TextFormat("%.0f", g->paddleSpeed),
                         settingsControlCenterX(), settingsRowCenterY(2), 18, 0.5f, WIZARD_BURGUNDY);

    const char *difficulty[3] = {"EASY", "NORMAL", "HARD"};
    for (int i = 0; i < 3; i++)
    {
        Rectangle rec = settingsDifficultyRect(i);
        drawWizardButton(a->buttonPlate, g->font, rec, difficulty[i], 12,
                         g->difficulty == i || CheckCollisionPointRec(mouse, rec));
    }

    drawBackButton(g, a, back, "BACK");
}


static void drawMenuTagline(Game *g, const char *text, float cy)
{
    const float size = 13.0f;
    const float spacing = 3.4f;
    const float cx = SCREEN_W / 2.0f;
    float half = MeasureTextEx(g->font, text, size, spacing).x / 2.0f;

    drawOutlinedCenteredText(g->font, text, cx, cy, size, spacing,
                             ASTRAL_BRASS_HI, Fade(ASTRAL_VOID, 0.55f));


    const float inner = half + 18.0f;
    const float length = 62.0f;
    DrawRectangleGradientH((int)(cx - inner - length), (int)cy, (int)length, 1,
                           Fade(ASTRAL_BRASS, 0.0f), Fade(ASTRAL_BRASS, 0.85f));
    DrawRectangleGradientH((int)(cx + inner), (int)cy, (int)length, 1,
                           Fade(ASTRAL_BRASS, 0.85f), Fade(ASTRAL_BRASS, 0.0f));
    DrawPoly((Vector2){cx - inner, cy + 0.5f}, 4, 3.0f, 0.0f, ASTRAL_BRASS_HI);
    DrawPoly((Vector2){cx + inner, cy + 0.5f}, 4, 3.0f, 0.0f, ASTRAL_BRASS_HI);
}

static void drawMainMenu(Game *g, Assets *a)
{
    drawMenuBackdrop(a, 0.15f);
    if (a->logo.id != 0)
        drawCenteredTexture(a->logo, 400, 101, 360, 180);
    else
        drawCenteredFontText(g->titleFont, "DX BALL", 400, 101, 46, 1.0f, ASTRAL_BRASS_HI);

    drawMenuTagline(g, "A WIZARDING BRICK-BREAKER", 186);

    static const char *labels[5] = {"PLAY", "HOW TO PLAY", "SETTINGS", "HIGH SCORES", "QUIT"};
    Vector2 mouse = getMouseDesignPosition();
    for (int i = 0; i < 5; i++)
    {
        Rectangle rec = mainMenuButtonRect(i);
        drawWizardButton(a->buttonPlate, g->font, rec, labels[i], 16, CheckCollisionPointRec(mouse, rec));
    }

    Rectangle credits = creditsButtonRect();
    drawWizardButton(a->buttonPlate, g->font, credits, "CREDITS", 11,
                     CheckCollisionPointRec(mouse, credits));

    const char *hint = "ENTER TO BEGIN";
    drawSolidFontText(g->font, hint,
                      (Vector2){18, menuHintCenterY() - MeasureTextEx(g->font, hint, 11, 1.2f).y / 2.0f},
                      11, 1.2f, ASTRAL_TEXT);
}


static void drawLevelTile(Game *g, Assets *a, int index, Vector2 hover)
{
    Rectangle tile = levelSelectTileRect(index);
    int unlocked = (index + 1 <= g->unlockedLevel);
    int cleared = (index + 1 < g->unlockedLevel);
    int hot = unlocked && CheckCollisionPointRec(hover, tile);

    Rectangle visual = tile;
    if (hot)
        visual.y -= 4.0f;

    const float roundness = 0.06f;
    DrawRectangleRounded((Rectangle){visual.x + 3, visual.y + 5, visual.width, visual.height},
                         roundness, 6, Fade(BLACK, 0.28f));
    DrawRectangleRounded(visual, roundness, 6, (Color){61, 31, 26, 255});

    Rectangle image = {visual.x + 4, visual.y + 4, visual.width - 8, 84};
    if (a->bg[index].id != 0)
        drawTextureCover(a->bg[index], image, WHITE);
    else
        DrawRectangleRec(image, ASTRAL_NAVY);
    DrawRectangleRec(image, Fade(ASTRAL_VOID, unlocked ? (hot ? 0.06f : 0.18f) : 0.82f));


    DrawRectangleRoundedLinesEx(visual, roundness, 6, 5.0f, (Color){61, 31, 26, 255});
    DrawRectangleRoundedLinesEx(visual, roundness, 6, hot ? 2.8f : 1.5f,
                         unlocked ? (hot ? ASTRAL_BRASS_HI : ASTRAL_BRASS) : Fade(ASTRAL_MUTED, 0.45f));

    float cx = visual.x + visual.width / 2.0f;


    const float inset = 7.0f;
    const float badgeRadius = 11.5f;
    const float capY = visual.y + inset + badgeRadius;
    Vector2 badge = {visual.x + inset + badgeRadius, capY};
    DrawCircleV(badge, badgeRadius - 1.0f, Fade(ASTRAL_VOID, 0.86f));
    DrawRing(badge, badgeRadius - 1.5f, badgeRadius, 0, 360, 28, unlocked ? ASTRAL_BRASS_HI : ASTRAL_MUTED);
    drawCenteredFontText(g->font, TextFormat("%d", index + 1), badge.x, badge.y, 12.5f, 0.1f,
                         unlocked ? ASTRAL_TEXT : ASTRAL_MUTED);

    if (unlocked)
    {
        if (cleared)
        {
            const float tagW = 52.0f, tagH = 2.0f * badgeRadius;
            Rectangle clearTag = {visual.x + visual.width - inset - tagW, capY - tagH / 2.0f, tagW, tagH};
            DrawRectangleRounded(clearTag, 0.4f, 6, Fade(ASTRAL_VOID, 0.82f));
            drawCenteredFontText(g->font, "CLEAR", clearTag.x + clearTag.width / 2, capY,
                                 9.5f, 0.3f, ASTRAL_SUCCESS);
        }
        int best = g->levelHighScores[index];
        drawCenteredFontText(g->font, best > 0 ? TextFormat("BEST  %d", best) : "NOT PLAYED",
                             cx, visual.y + 117, 10.5f, 0.15f,
                             best > 0 ? ASTRAL_BRASS_HI : ASTRAL_MUTED);
    }
    else
        drawCenteredFontText(g->font, "LOCKED", cx, image.y + image.height / 2.0f, 13, 0.6f, ASTRAL_MUTED);

    static const char *shortNames[TOTAL_LEVELS] = {
        "CHESS", "SECRETS", "PATRONUS", "DRAGON", "PROPHECY", "HORCRUX", "HOGWARTS"};


    drawCenteredFontText(g->font, shortNames[index], cx, visual.y + 100, 12.5f, 0.25f,
                         unlocked ? ASTRAL_TEXT : Fade(ASTRAL_MUTED, 0.45f));
}

static void drawLevelSelect(Game *g, Assets *a)
{
    drawGeneratedBackdrop(a->levelSelectBackground, a, 0.0f);
    drawCenteredFontText(g->titleFont, "CHOOSE A CHAMBER", 400, 113, 26, 0.8f, WIZARD_BURGUNDY);
    drawCenteredFontText(g->font, TextFormat("WITCH OR WIZARD: %s", g->playerName),
                         400, 141, 12.5f, 0.8f, WIZARD_INK_MUTED);
    Vector2 hover = getMouseDesignPosition();
    int hovered = -1;
    for (int i = 0; i < TOTAL_LEVELS; i++)
    {
        drawLevelTile(g, a, i, hover);
        if (i + 1 <= g->unlockedLevel && CheckCollisionPointRec(hover, levelSelectTileRect(i)))
            hovered = i;
    }

    int unlocked = g->unlockedLevel > TOTAL_LEVELS ? TOTAL_LEVELS : g->unlockedLevel;


    drawCenteredFontText(g->titleFont, hovered >= 0 ? levelName(hovered + 1) : "SELECT A CHAMBER",
                         400, 486, 16, 0.25f, ASTRAL_BRASS_HI);
    drawCenteredFontText(g->font, TextFormat("%d OF %d CHAMBERS UNLOCKED", unlocked, TOTAL_LEVELS),
                         400, 508, 12, 0.6f, ASTRAL_TEXT);
    drawBackButton(g, a, bottomActionRect(0, 1), "BACK");
}


static void drawCredits(Game *g, Assets *a)
{
    drawGeneratedBackdrop(a->settingsBackground, a, 0.05f);

    drawCenteredFontText(g->titleFont, "DX BALL", SCREEN_W / 2.0f, 108, 27, 0.8f, WIZARD_BURGUNDY);
    drawCenteredFontText(g->font, "WIZARDING EDITION", SCREEN_W / 2.0f, 133, 14, 1.0f, WIZARD_INK_MUTED);


    drawCenteredFontText(g->font, "GAME DESIGN AND CODE", SCREEN_W / 2.0f,
                         settingsRowCenterY(0), 15, 0.7f, WIZARD_BURGUNDY);

    drawCenteredFontText(g->font, "ASHIFA J. RAHMAN(2505106)", SCREEN_W / 2.0f, 220, 14, 0.45f, WIZARD_INK);
    drawCenteredFontText(g->font, "JOYEETA MITRA(2505100)", SCREEN_W / 2.0f, 242, 14, 0.45f, WIZARD_INK);

    drawCenteredFontText(g->font, "SOUND AND ASSETS", SCREEN_W / 2.0f,
                         settingsRowCenterY(2), 15, 0.7f, WIZARD_BURGUNDY);


    const float detailLabelX = 300.0f;
    const float detailValueX = 455.0f;
    const float detailSize = 12.5f;
    drawCenteredFontText(g->font, "BG MUSIC", detailLabelX, 324, detailSize, 0.3f, WIZARD_INK);
    drawCenteredFontText(g->font, "HARRY POTTER SOUNDTRACKS", detailValueX, 324, detailSize, 0.15f, WIZARD_INK);
    drawCenteredFontText(g->font, "SFX", detailLabelX, 343, detailSize, 0.3f, WIZARD_INK);
    drawCenteredFontText(g->font, "YOUTUBE", detailValueX, 343, detailSize, 0.3f, WIZARD_INK);
    drawCenteredFontText(g->font, "ASSETS", detailLabelX, 362, detailSize, 0.3f, WIZARD_INK);
    drawCenteredFontText(g->font, "HARRY POTTER THEMED", detailValueX, 362, detailSize, 0.25f, WIZARD_INK);

    drawCenteredFontText(g->font, "UNDER THE SUPERVISION OF", SCREEN_W / 2.0f,
                         settingsRowCenterY(4), 14, 0.55f, WIZARD_BURGUNDY);
    drawCenteredFontText(g->font, "ANWARUL BASHIR SHUAIB", SCREEN_W / 2.0f,
                         445, 13.5f, 0.35f, WIZARD_INK);

    drawBackButton(g, a, bottomActionRect(0, 1), "BACK");
}


static void drawResumePrompt(Game *g, Assets *a)
{
    drawMenuBackdrop(a, 0.78f);
    drawAstralPanel((Rectangle){175, 170, 450, 245}, 0.98f);
    drawCenteredFontText(g->font, TextFormat("WELCOME BACK, %s", g->playerName), SCREEN_W / 2.0f, 218, 24, 1.0f, ASTRAL_BRASS_HI);
    drawCenteredFontText(g->font, TextFormat("RETURN TO CHAMBER %d?", g->savedLevel), SCREEN_W / 2.0f, 265, 17, 0.7f, ASTRAL_TEXT);
    drawAstralRule(SCREEN_W / 2.0f, 300, 180);
    Vector2 mouse = getMouseDesignPosition();
    Rectangle yes = resumePromptButtonRect(0), no = resumePromptButtonRect(1);
    drawWizardButton(a->buttonPlate, g->font, yes, "CONTINUE", 15, CheckCollisionPointRec(mouse, yes));
    drawWizardButton(a->buttonPlate, g->font, no, "PICK LEVEL", 15, CheckCollisionPointRec(mouse, no));
}

static void drawNameEntry(Game *g, Assets *a)
{
    drawMenuBackdrop(a, 0.78f);


    drawAstralPanel((Rectangle){190, 170, 420, 250}, 0.98f);
    drawCenteredFontText(g->titleFont, "ENTER YOUR WIZARD NAME", SCREEN_W / 2.0f, 227, 22, 0.8f, ASTRAL_BRASS_HI);
    Rectangle field = {250, 274, 300, 56};
    float fieldMidY = field.y + field.height / 2.0f;
    DrawRectangleRounded(field, 0.24f, 8, Fade(ASTRAL_VOID, 0.85f));
    DrawRectangleRoundedLinesEx(field, 0.24f, 8, 1.5f, ASTRAL_BRASS);
    const char *shown = g->playerName[0] == '\0' ? "" : g->playerName;
    drawCenteredFontText(g->font, shown, SCREEN_W / 2.0f, fieldMidY, 24, 0.8f, ASTRAL_TEXT);
    if (((int)(GetTime() * 2)) % 2 == 0)
    {
        Vector2 size = MeasureTextEx(g->font, shown, 24, 0.8f);
        DrawRectangle((int)(SCREEN_W / 2.0f + size.x / 2 + 4), (int)(fieldMidY - 11.5f), 2, 23, ASTRAL_TEAL);
    }
    drawCenteredFontText(g->font, "TYPE A NAME AND PRESS ENTER", SCREEN_W / 2.0f, 366, 13, 1.0f, ASTRAL_MUTED);
}

void drawMenuScreens(Game *g, Assets *a)
{
    if (g->showHowToPlay)
        drawHowToPlay(g, a);
    else if (g->showHighScore)
        drawHighScoreBoard(g, a);
    else if (g->showSettings)
        drawSettingsScreen(g, a);
    else if (g->showCredits)
        drawCredits(g, a);
    else
        drawMainMenu(g, a);

    if (g->showLevelSelect)
        drawLevelSelect(g, a);
    if (g->showResumePrompt)
        drawResumePrompt(g, a);
    if (g->showNameEntry)
        drawNameEntry(g, a);
}
