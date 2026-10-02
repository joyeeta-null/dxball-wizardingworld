#ifndef RENDER_H
#define RENDER_H

#include "raylib.h"
#include "config.h"
#include "types.h"


float getRenderScale(void);

Vector2 getRenderOffset(void);

Vector2 getMouseDesignPosition(void);

void drawTextureFit(Texture2D tex, float x, float y, float w, float h);


void drawTextureCover(Texture2D tex, Rectangle destination, Color tint);

void drawCenteredTexture(Texture2D tex, float cx, float cy, float w, float h);

void drawLabeledButton(Texture2D tex, float cx, float cy, float w, float h, const char *text, int fontSize, Color color);

void drawSolidFontText(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color color);

void drawCenteredFontText(Font font, const char *text, float cx, float cy, float fontSize, float spacing, Color color);

void drawLabeledButtonFont(Texture2D tex, Font font, float cx, float cy, float w, float h, const char *text, float fontSize, Color color);

void drawAstralPanel(Rectangle rec, float opacity);

void drawAstralButton(Font font, Rectangle rec, const char *label, float fontSize, int highlighted);

void drawWizardButton(Texture2D plate, Font font, Rectangle rec, const char *label,
                      float fontSize, int highlighted);


void drawOutlinedCenteredText(Font font, const char *text, float cx, float cy,
                              float fontSize, float spacing, Color color, Color outline);

void drawAstralRule(float cx, float y, float width);

Rectangle levelSelectTileRect(int index);

Rectangle mainMenuButtonRect(int index);

Rectangle creditsButtonRect(void);


float menuHintCenterY(void);


Rectangle bottomActionRect(int index, int total);

typedef enum
{
    HELP_NAV_PREVIOUS,
    HELP_NAV_BACK,
    HELP_NAV_NEXT
} HelpNav;


Rectangle helpNavRect(int page, HelpNav which);


Rectangle resumePromptButtonRect(int index);


float settingsRowCenterY(int row);

float settingsLabelX(void);


float settingsControlCenterX(void);

Rectangle settingsToggleRect(int row);


Rectangle settingsStepRect(int row, int side);

Rectangle settingsDifficultyRect(int index);

Rectangle pauseMenuButtonRect(int index);

Rectangle hudPauseButtonRect(void);


void presentScreen(RenderTexture2D screen);

#endif
