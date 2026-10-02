#include "raylib.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bricks.h"
#include "render.h"
#include "theme.h"


int brickMaxHP(BrickType type)
{
    switch (type)
    {
    case BRICK_ICE:
    case BRICK_FIRE:
    case BRICK_LIGHTNING:
    case BRICK_RUNE:
    case BRICK_WOOD:
        return 2;
    case BRICK_STONE:
    case BRICK_LOCKED:
        return 3;
    default:
        return 1;
    }
}

static Color brickFallbackColor(BrickType type)
{
    switch (type)
    {
    case BRICK_RED:
        return RED;
    case BRICK_BLUE:
        return BLUE;
    case BRICK_GREEN:
        return GREEN;
    case BRICK_YELLOW:
        return YELLOW;
    case BRICK_PURPLE:
        return PURPLE;
    case BRICK_ICE:
        return SKYBLUE;
    case BRICK_FIRE:
        return ORANGE;
    case BRICK_LIGHTNING:
        return GOLD;
    case BRICK_RUNE:
        return VIOLET;
    case BRICK_WOOD:
        return BROWN;
    case BRICK_STONE:
        return DARKGRAY;
    case BRICK_LOCKED:
        return GRAY;
    case BRICK_SKULL:
        return WHITE;
    case BRICK_BOOK:
        return MAGENTA;
    default:
        return WHITE;
    }
}

static Texture2D *getBrickTexture(Assets *a, BrickType type, int hp)
{
    switch (type)
    {
    case BRICK_RED:
        return &a->red;
    case BRICK_BLUE:
        return &a->blue;
    case BRICK_GREEN:
        return &a->green;
    case BRICK_YELLOW:
        return &a->yellow;
    case BRICK_PURPLE:
        return &a->purple;

    case BRICK_ICE:
        return &a->ice;
    case BRICK_FIRE:
        return &a->fire;
    case BRICK_LIGHTNING:
        return &a->lightning;
    case BRICK_RUNE:
        return &a->rune;
    case BRICK_WOOD:
        return &a->wood;

    case BRICK_STONE:
        if (hp == 2)
            return &a->stoneHit1;
        if (hp == 1)
            return &a->stoneHit2;
        return &a->stone;

    case BRICK_LOCKED:
        if (hp == 2)
            return &a->lockedHit1;
        if (hp == 1)
            return &a->lockedHit2;
        return &a->locked;

    case BRICK_SKULL:
        return &a->skull;
    case BRICK_BOOK:
        return &a->book;
    default:
        return NULL;
    }
}

static Texture2D *getBreakTexture(Assets *a, BrickType type)
{
    switch (type)
    {
    case BRICK_RED:
        return &a->redBreak;
    case BRICK_BLUE:
        return &a->blueBreak;
    case BRICK_GREEN:
        return &a->greenBreak;
    case BRICK_YELLOW:
        return &a->goldBreak;
    case BRICK_PURPLE:
        return &a->purpleBreak;
    case BRICK_ICE:
        return &a->iceBreak;
    case BRICK_FIRE:
        return &a->fireBreak;
    case BRICK_LIGHTNING:
        return &a->lightningBreak;
    case BRICK_RUNE:
        return &a->runeBreak;
    case BRICK_WOOD:
        return &a->woodBreak;
    case BRICK_STONE:
        return &a->stoneBreak;
    case BRICK_LOCKED:
        return &a->lockedBreak;
    case BRICK_SKULL:
        return &a->skullBreak;
    case BRICK_BOOK:
        return &a->bookBreak;
    default:
        return NULL;
    }
}

void initBrick(Brick *b, BrickType type, float x, float y, float w, float h)
{
    b->x = x;
    b->y = y;
    b->w = w;
    b->h = h;
    b->type = type;
    b->hp = brickMaxHP(type);
    b->active = (type != BRICK_NONE);
}

void clearBricks(Brick bricks[ROWS][COLS])
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            initBrick(&bricks[r][c], BRICK_NONE, 0, 0, 0, 0);
}


static const Rectangle BRICK_ATLAS_UV[14] = {
    {0.15944f, 0.01525f, 0.30087f, 0.12011f},
    {0.53969f, 0.01430f, 0.30087f, 0.12107f},
    {0.15877f, 0.14871f, 0.30487f, 0.12202f},
    {0.53702f, 0.14967f, 0.30487f, 0.12107f},
    {0.15744f, 0.28408f, 0.30554f, 0.12202f},
    {0.53436f, 0.28503f, 0.31087f, 0.12107f},
    {0.15143f, 0.41849f, 0.31688f, 0.12202f},
    {0.53169f, 0.41849f, 0.31621f, 0.12202f},
    {0.15277f, 0.55291f, 0.31288f, 0.12202f},
    {0.53169f, 0.55195f, 0.31554f, 0.12297f},
    {0.15077f, 0.68732f, 0.31755f, 0.12297f},
    {0.52969f, 0.68732f, 0.31888f, 0.12393f},
    {0.14610f, 0.82173f, 0.32688f, 0.12488f},
    {0.53169f, 0.82269f, 0.31755f, 0.12393f},
};

static Rectangle brickAtlasSource(Assets *a, BrickType type)
{
    const Rectangle uv = BRICK_ATLAS_UV[(int)type - 1];
    float w = (float)a->brickAtlas.width;
    float h = (float)a->brickAtlas.height;

    return (Rectangle){uv.x * w, uv.y * h, uv.width * w, uv.height * h};
}

void drawBrickSprite(Assets *a, BrickType type, Rectangle destination, Color tint)
{
    if (a->brickAtlas.id == 0 || type <= BRICK_NONE || type > BRICK_BOOK)
        return;

    DrawTexturePro(a->brickAtlas, brickAtlasSource(a, type), destination,
                   (Vector2){0, 0}, 0.0f, tint);
}

int remainingBricks(Brick bricks[ROWS][COLS])
{
    int count = 0;
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if (bricks[r][c].active)
                count++;
    return count;
}

void drawBricks(Brick bricks[ROWS][COLS], Assets *a)
{
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            Brick *b = &bricks[r][c];
            if (!b->active)
                continue;

            Rectangle destination = {b->x, b->y, b->w, b->h};
            if (a->brickAtlas.id != 0)
            {
                drawBrickSprite(a, b->type, destination, WHITE);

                int damage = brickMaxHP(b->type) - b->hp;
                if (damage > 0)
                {
                    Color crack = Fade(ASTRAL_VOID, 0.74f);
                    DrawLineEx((Vector2){b->x + b->w * 0.30f, b->y + 2},
                               (Vector2){b->x + b->w * 0.56f, b->y + b->h - 2}, 1.2f, crack);
                    if (damage > 1)
                        DrawLineEx((Vector2){b->x + b->w * 0.70f, b->y + 2},
                                   (Vector2){b->x + b->w * 0.44f, b->y + b->h - 2}, 1.2f, crack);
                }
            }
            else
            {
                Texture2D *t = getBrickTexture(a, b->type, b->hp);
                if (t && t->id != 0)
                    drawTextureFit(*t, b->x, b->y, b->w, b->h);
                else
                    DrawRectangleRec(destination, brickFallbackColor(b->type));
            }
        }
    }
}

void addBreakFX(BreakFX fx[], float x, float y, float w, float h, BrickType type)
{
    for (int i = 0; i < MAX_BREAK_FX; i++)
    {
        if (!fx[i].active)
        {
            fx[i].x = x;
            fx[i].y = y;
            fx[i].w = w;
            fx[i].h = h;
            fx[i].type = type;
            fx[i].timer = 0.22f;
            fx[i].active = 1;
            return;
        }
    }
}

void updateBreakFX(BreakFX fx[], float dt)
{
    for (int i = 0; i < MAX_BREAK_FX; i++)
    {
        if (fx[i].active)
        {
            fx[i].timer -= dt;
            if (fx[i].timer <= 0)
                fx[i].active = 0;
        }
    }
}

void drawBreakFX(BreakFX fx[], Assets *a)
{
    for (int i = 0; i < MAX_BREAK_FX; i++)
    {
        if (!fx[i].active)
            continue;
        float alpha = fx[i].timer / 0.22f;
        float spread = (1.0f - alpha) * 10.0f;

        if (a->brickAtlas.id != 0)
        {
            Rectangle dst = {fx[i].x - spread / 2.0f, fx[i].y - spread / 3.0f,
                             fx[i].w + spread, fx[i].h + spread * 0.65f};
            drawBrickSprite(a, fx[i].type, dst, Fade(WHITE, alpha));
        }
        else
        {
            Texture2D *t = getBreakTexture(a, fx[i].type);
            if (t && t->id != 0)
            {
                Color tint = Fade(WHITE, alpha);
                Rectangle src = {0, 0, (float)t->width, (float)t->height};
                Rectangle dst = {fx[i].x, fx[i].y, fx[i].w, fx[i].h};
                DrawTexturePro(*t, src, dst, (Vector2){0, 0}, 0, tint);
            }
        }
    }
}
