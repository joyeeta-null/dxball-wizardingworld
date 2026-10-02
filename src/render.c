#include "raylib.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "render.h"
#include "theme.h"


float getRenderScale(void)
{
    float scaleX = (float)GetScreenWidth() / SCREEN_W;
    float scaleY = (float)GetScreenHeight() / SCREEN_H;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;


    return (scale > 0.0f && isfinite(scale)) ? scale : 1.0f;
}


Vector2 getRenderOffset(void)
{
    float scale = getRenderScale();
    return (Vector2){(GetScreenWidth() - SCREEN_W * scale) / 2.0f,
                     (GetScreenHeight() - SCREEN_H * scale) / 2.0f};
}


Vector2 getMouseDesignPosition(void)
{
    float scale = getRenderScale();
    Vector2 offset = getRenderOffset();
    Vector2 mouse = GetMousePosition();
    return (Vector2){(mouse.x - offset.x) / scale, (mouse.y - offset.y) / scale};
}

void drawTextureFit(Texture2D tex, float x, float y, float w, float h)
{
    if (tex.id == 0)
        return;

    Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
    Rectangle dst = {x, y, w, h};
    DrawTexturePro(tex, src, dst, (Vector2){0, 0}, 0.0f, WHITE);
}

void drawCenteredTexture(Texture2D tex, float cx, float cy, float w, float h)
{
    drawTextureFit(tex, cx - w / 2, cy - h / 2, w, h);
}

void drawLabeledButton(Texture2D tex, float cx, float cy, float w, float h, const char *text, int fontSize, Color color)
{
    drawCenteredTexture(tex, cx, cy, w, h);
    int tw = MeasureText(text, fontSize);
    DrawText(text, (int)(cx - tw / 2), (int)(cy - fontSize / 2), fontSize, color);
}

void drawSolidFontText(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color color)
{
    DrawTextEx(font, text, (Vector2){position.x - 0.55f, position.y}, fontSize, spacing, color);
    DrawTextEx(font, text, (Vector2){position.x + 0.55f, position.y}, fontSize, spacing, color);
    DrawTextEx(font, text, position, fontSize, spacing, color);
}

void drawCenteredFontText(Font font, const char *text, float cx, float cy, float fontSize, float spacing, Color color)
{
    Vector2 size = MeasureTextEx(font, text, fontSize, spacing);
    drawSolidFontText(font, text, (Vector2){cx - size.x / 2.0f, cy - size.y / 2.0f}, fontSize, spacing, color);
}

void drawLabeledButtonFont(Texture2D tex, Font font, float cx, float cy, float w, float h, const char *text, float fontSize, Color color)
{
    drawCenteredTexture(tex, cx, cy, w, h);
    drawCenteredFontText(font, text, cx, cy, fontSize, 1.0f, color);
}


Rectangle levelSelectTileRect(int index)
{
    if (index < 0)
        index = 0;
    if (index >= TOTAL_LEVELS)
        index = TOTAL_LEVELS - 1;


    const float tileW = 142.0f;
    const float tileH = 132.0f;
    const float gap = 14.0f;
    int count = (index < 4) ? 4 : TOTAL_LEVELS - 4;
    int column = (index < 4) ? index : index - 4;
    float rowWidth = count * tileW + (count - 1) * gap;
    float startX = (SCREEN_W - rowWidth) / 2.0f;
    float rowY = (index < 4) ? 165.0f : 165.0f + tileH + 21.0f;
    return (Rectangle){startX + column * (tileW + gap), rowY, tileW, tileH};
}

void drawTextureCover(Texture2D tex, Rectangle destination, Color tint)
{
    if (tex.id == 0 || destination.width <= 0 || destination.height <= 0)
        return;

    float sourceAspect = (float)tex.width / (float)tex.height;
    float destinationAspect = destination.width / destination.height;
    Rectangle source = {0, 0, (float)tex.width, (float)tex.height};

    if (sourceAspect > destinationAspect)
    {
        source.width = tex.height * destinationAspect;
        source.x = (tex.width - source.width) / 2.0f;
    }
    else if (sourceAspect < destinationAspect)
    {
        source.height = tex.width / destinationAspect;
        source.y = (tex.height - source.height) / 2.0f;
    }

    DrawTexturePro(tex, source, destination, (Vector2){0, 0}, 0.0f, tint);
}

Rectangle mainMenuButtonRect(int index)
{
    return (Rectangle){285, 204 + index * 61.0f, 230, 52};
}


Rectangle creditsButtonRect(void)
{
    const float w = 136.0f, h = 38.0f;
    return (Rectangle){SCREEN_W - 18.0f - w, 580.0f - h, w, h};
}


float menuHintCenterY(void)
{
    Rectangle credits = creditsButtonRect();
    return credits.y + credits.height / 2.0f;
}

Rectangle bottomActionRect(int index, int total)
{
    const float width = 176.0f;
    const float height = 46.0f;
    const float gap = 34.0f;

    if (total < 1)
        total = 1;
    if (total > 3)
        total = 3;
    if (index < 0)
        index = 0;
    if (index >= total)
        index = total - 1;

    float rowWidth = total * width + (total - 1) * gap;
    float startX = (SCREEN_W - rowWidth) / 2.0f;
    return (Rectangle){startX + index * (width + gap), 534.0f, width, height};
}


Rectangle helpNavRect(int page, HelpNav which)
{
    int hasPrevious = (page > 0);
    int hasNext = (page < 2);
    int total = 1 + hasPrevious + hasNext;

    if (which == HELP_NAV_PREVIOUS)
        return hasPrevious ? bottomActionRect(0, total) : (Rectangle){0, 0, 0, 0};
    if (which == HELP_NAV_NEXT)
        return hasNext ? bottomActionRect(total - 1, total) : (Rectangle){0, 0, 0, 0};
    return bottomActionRect(hasPrevious ? 1 : 0, total);
}


Rectangle resumePromptButtonRect(int index)
{
    const float w = 170.0f, h = 50.0f, gap = 34.0f;
    float startX = (SCREEN_W - (2.0f * w + gap)) / 2.0f;
    return (Rectangle){startX + (index ? 1 : 0) * (w + gap), 325.0f, w, h};
}


#define SETTINGS_ROW_Y0 175.4f
#define SETTINGS_ROW_STEP 55.95f
#define SETTINGS_LABEL_X 196.0f

#define SETTINGS_CONTROL_CX 485.0f

static int clampRow(int row)
{
    if (row < 0)
        return 0;
    if (row > 4)
        return 4;
    return row;
}

float settingsRowCenterY(int row)
{
    return SETTINGS_ROW_Y0 + clampRow(row) * SETTINGS_ROW_STEP;
}

float settingsLabelX(void)
{
    return SETTINGS_LABEL_X;
}

float settingsControlCenterX(void)
{
    return SETTINGS_CONTROL_CX;
}

Rectangle settingsToggleRect(int row)
{
    const float w = 128.0f, h = 42.0f;
    return (Rectangle){SETTINGS_CONTROL_CX - w / 2.0f, settingsRowCenterY(row) - h / 2.0f, w, h};
}

Rectangle settingsStepRect(int row, int side)
{
    const float d = 38.0f;
    const float offset = (side ? 74.0f : -74.0f);
    return (Rectangle){SETTINGS_CONTROL_CX + offset - d / 2.0f,
                       settingsRowCenterY(row) - d / 2.0f, d, d};
}

Rectangle settingsDifficultyRect(int index)
{


    const float w = 82.0f, h = 38.0f, gap = 7.0f;
    if (index < 0)
        index = 0;
    if (index > 2)
        index = 2;
    float rowWidth = 3.0f * w + 2.0f * gap;
    float startX = SETTINGS_CONTROL_CX - rowWidth / 2.0f;
    return (Rectangle){startX + index * (w + gap), settingsRowCenterY(3) - h / 2.0f, w, h};
}

Rectangle pauseMenuButtonRect(int index)
{
    if (index < 0)
        index = 0;
    if (index > 2)
        index = 2;
    return (Rectangle){275.0f, 231.0f + index * 74.0f, 250.0f, 56.0f};
}

Rectangle hudPauseButtonRect(void)
{
    return (Rectangle){HUD_PAUSE_BUTTON_X - 22.0f, HUD_PAUSE_BUTTON_Y - 20.0f, 44.0f, 40.0f};
}

void drawAstralPanel(Rectangle rec, float opacity)
{
    DrawRectangleRounded((Rectangle){rec.x + 5, rec.y + 7, rec.width, rec.height},
                         0.10f, 10, Fade(BLACK, 0.38f * opacity));
    DrawRectangleRounded(rec, 0.10f, 10, Fade(ASTRAL_PANEL, opacity));
    DrawRectangleRoundedLinesEx(rec, 0.10f, 10, 1.5f, Fade(ASTRAL_BRASS, 0.76f * opacity));
    DrawLineEx((Vector2){rec.x + 20, rec.y + 8},
               (Vector2){rec.x + rec.width - 20, rec.y + 8},
               1.0f, Fade(ASTRAL_BRASS_HI, 0.24f * opacity));
}

void drawAstralButton(Font font, Rectangle rec, const char *label, float fontSize, int highlighted)
{
    Color fill = highlighted ? Fade(ASTRAL_TEAL, 0.30f) : Fade(ASTRAL_PANEL_2, 0.92f);
    Color edge = highlighted ? ASTRAL_BRASS_HI : Fade(ASTRAL_BRASS, 0.78f);

    DrawRectangleRounded((Rectangle){rec.x + 2, rec.y + 3, rec.width, rec.height},
                         0.22f, 8, Fade(BLACK, 0.34f));
    DrawRectangleRounded(rec, 0.22f, 8, fill);
    DrawRectangleRoundedLinesEx(rec, 0.22f, 8, highlighted ? 2.0f : 1.2f, edge);

    float cy = rec.y + rec.height / 2.0f;
    DrawCircleV((Vector2){rec.x + 13, cy}, 2.2f, edge);
    DrawCircleV((Vector2){rec.x + rec.width - 13, cy}, 2.2f, edge);
    drawCenteredFontText(font, label, rec.x + rec.width / 2.0f, cy, fontSize, 1.0f,
                         highlighted ? ASTRAL_TEXT : ASTRAL_BRASS_HI);
}


static void drawPlateSlices(Texture2D plate, Rectangle rec, Color tint)
{

    float sourceCap = plate.width * 0.18f;
    float destinationCap = rec.height * (sourceCap / plate.height);
    if (destinationCap * 2.0f > rec.width * 0.72f)
        destinationCap = rec.width * 0.36f;

    float sourceOrnament = plate.width * 0.12f;
    float destinationOrnament = rec.height * (sourceOrnament / plate.height);
    float sourceStretch = (plate.width - sourceCap * 2.0f - sourceOrnament) / 2.0f;
    float destinationStretch = (rec.width - destinationCap * 2.0f - destinationOrnament) / 2.0f;

    float sourceX[5] = {0, sourceCap, sourceCap + sourceStretch,
                        sourceCap + sourceStretch + sourceOrnament,
                        plate.width - sourceCap};
    float sourceW[5] = {sourceCap, sourceStretch, sourceOrnament, sourceStretch, sourceCap};
    float destinationX[5] = {rec.x, rec.x + destinationCap,
                             rec.x + destinationCap + destinationStretch,
                             rec.x + destinationCap + destinationStretch + destinationOrnament,
                             rec.x + rec.width - destinationCap};
    float destinationW[5] = {destinationCap, destinationStretch, destinationOrnament,
                             destinationStretch, destinationCap};

    for (int i = 0; i < 5; i++)
        DrawTexturePro(plate,
                       (Rectangle){sourceX[i], 0, sourceW[i], (float)plate.height},
                       (Rectangle){destinationX[i], rec.y, destinationW[i], rec.height},
                       (Vector2){0, 0}, 0.0f, tint);
}

void drawWizardButton(Texture2D plate, Font font, Rectangle rec, const char *label,
                      float fontSize, int highlighted)
{
    if (plate.id == 0)
    {
        drawAstralButton(font, rec, label, fontSize, highlighted);
        return;
    }

    drawPlateSlices(plate, rec, highlighted ? WHITE : (Color){235, 229, 215, 255});


    if (highlighted)
    {
        BeginBlendMode(BLEND_ADDITIVE);
        drawPlateSlices(plate, rec, (Color){96, 74, 24, 255});
        EndBlendMode();
    }

    drawCenteredFontText(font, label, rec.x + rec.width / 2.0f,
                         rec.y + rec.height / 2.0f, fontSize, 0.8f,
                         highlighted ? (Color){255, 242, 184, 255} : ASTRAL_TEXT);
}


void drawOutlinedCenteredText(Font font, const char *text, float cx, float cy,
                              float fontSize, float spacing, Color color, Color outline)
{
    static const float ring[8][2] = {
        {-1.3f, 0.0f}, {1.3f, 0.0f}, {0.0f, -1.3f}, {0.0f, 1.3f},
        {-1.0f, -1.0f}, {1.0f, -1.0f}, {-1.0f, 1.0f}, {1.0f, 1.0f}};

    Vector2 extent = MeasureTextEx(font, text, fontSize, spacing);
    Vector2 origin = {cx - extent.x / 2.0f, cy - extent.y / 2.0f};

    for (int i = 0; i < 8; i++)
        DrawTextEx(font, text, (Vector2){origin.x + ring[i][0], origin.y + ring[i][1]},
                   fontSize, spacing, outline);
    DrawTextEx(font, text, origin, fontSize, spacing, color);
}

void drawAstralRule(float cx, float y, float width)
{
    DrawLineEx((Vector2){cx - width / 2.0f, y}, (Vector2){cx + width / 2.0f, y},
               1.0f, Fade(ASTRAL_BRASS, 0.68f));
    DrawPoly((Vector2){cx, y}, 4, 4.0f, 45.0f, ASTRAL_BRASS_HI);
}


void presentScreen(RenderTexture2D screen)
{
    float scale = getRenderScale();
    Vector2 offset = getRenderOffset();

    BeginDrawing();
    ClearBackground(BLACK);


    DrawTexturePro(
        screen.texture,
        (Rectangle){0, 0, (float)screen.texture.width, -(float)screen.texture.height},
        (Rectangle){offset.x, offset.y, SCREEN_W * scale, SCREEN_H * scale},
        (Vector2){0, 0},
        0.0f,
        WHITE);

    EndDrawing();
}
