#include "raylib.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "storage.h"
#include "bricks.h"


void ensureSaveDir(void)
{
    if (!DirectoryExists(SAVE_DIR))
        MakeDirectory(SAVE_DIR);
}

static const char *progressFile(const char *playerName)
{
    return TextFormat(SAVE_DIR "/progress_%s.txt", playerName);
}


static const char *unlockFile(const char *playerName)
{
    return TextFormat(SAVE_DIR "/unlocked_%s.txt", playerName);
}

static const char *levelScoresFile(const char *playerName)
{
    return TextFormat(SAVE_DIR "/level_scores_%s.txt", playerName);
}


void saveProgress(const char *playerName, int level, int score, int lives,
                  int levelStartScore, Brick bricks[ROWS][COLS])
{
    char buffer[1024];
    int initial = snprintf(buffer, sizeof(buffer), "DXSAVE2 %d %d %d %d\n",
                           level, score, lives, levelStartScore);
    if (initial < 0 || (size_t)initial >= sizeof(buffer))
        return;
    size_t used = (size_t)initial;

    if (bricks != NULL)
    {
        for (int r = 0; r < ROWS; r++)
        {
            for (int c = 0; c < COLS; c++)
            {
                int state = bricks[r][c].active ? bricks[r][c].hp : 0;
                int written = snprintf(buffer + used, sizeof(buffer) - used, "%d ", state);
                if (written < 0 || (size_t)written >= sizeof(buffer) - used)
                    return;
                used += (size_t)written;
            }
        }
    }

    SaveFileText(progressFile(playerName), buffer);
}

void clearProgress(const char *playerName)
{
    const char *path = progressFile(playerName);
    if (FileExists(path))
        remove(path);
}


int loadProgress(const char *playerName, int *level, int *score, int *lives,
                 int *levelStartScore, int *brickState)
{
    for (int i = 0; i < ROWS * COLS; i++)
        brickState[i] = -1;

    const char *path = progressFile(playerName);

    if (!FileExists(path))
        return 0;

    char *text = LoadFileText(path);
    if (text == NULL)
        return 0;

    int savedLevel = 0, savedScore = 0, savedLives = 0;
    int savedLevelStartScore = 0, offset = 0;
    int parsed = sscanf(text, "DXSAVE2 %d %d %d %d%n",
                        &savedLevel, &savedScore, &savedLives,
                        &savedLevelStartScore, &offset);


    if (parsed != 4)
    {
        parsed = sscanf(text, "%d %d %d%n",
                        &savedLevel, &savedScore, &savedLives, &offset);
        savedLevelStartScore = savedScore;
    }

    if ((parsed != 3 && parsed != 4) ||
        savedLevel < 1 || savedLevel > TOTAL_LEVELS || savedLives < 1 ||
        savedLevelStartScore < 0 || savedLevelStartScore > savedScore)
    {
        UnloadFileText(text);
        return 0;
    }

    const char *cursor = text + offset;
    int count = 0;
    while (count < ROWS * COLS)
    {
        int value = 0, consumed = 0;
        if (sscanf(cursor, "%d%n", &value, &consumed) != 1)
            break;

        brickState[count++] = value;
        cursor += consumed;
    }

    UnloadFileText(text);

    if (count != ROWS * COLS)
    {
        for (int i = 0; i < ROWS * COLS; i++)
            brickState[i] = -1;
    }

    *level = savedLevel;
    *score = savedScore;
    *lives = savedLives;
    *levelStartScore = savedLevelStartScore;
    return 1;
}

void applyBrickState(Brick bricks[ROWS][COLS], const int *brickState)
{
    if (brickState[0] < 0)
        return;

    int i = 0;
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            int state = brickState[i++];

            if (state <= 0)
                bricks[r][c].active = 0;
            else if (bricks[r][c].active)
            {


                int maxHP = brickMaxHP(bricks[r][c].type);
                bricks[r][c].hp = (state > maxHP) ? maxHP : state;
            }
        }
    }
}

int loadUnlockedLevel(const char *playerName)
{
    if (playerName == NULL || playerName[0] == '\0')
        return 1;

    const char *path = unlockFile(playerName);
    if (!FileExists(path))
        return 1;

    char *text = LoadFileText(path);
    if (text == NULL)
        return 1;

    int value = 1;
    if (sscanf(text, "%d", &value) != 1)
        value = 1;
    UnloadFileText(text);

    if (value < 1)
        value = 1;

    if (value > TOTAL_LEVELS + 1)
        value = TOTAL_LEVELS + 1;
    return value;
}

void saveUnlockedLevel(const char *playerName, int level)
{
    if (playerName == NULL || playerName[0] == '\0')
        return;

    if (level < 1)
        level = 1;
    if (level > TOTAL_LEVELS + 1)
        level = TOTAL_LEVELS + 1;

    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d", level);
    SaveFileText(unlockFile(playerName), buffer);
}

int loadHighScores(ScoreEntry list[])
{
    memset(list, 0, sizeof(ScoreEntry) * MAX_SCORES);

    char *text = FileExists(SAVE_DIR "/highscores.txt") ? LoadFileText(SAVE_DIR "/highscores.txt") : NULL;
    if (text == NULL)
        return 0;

    int count = 0, consumed = 0;
    const char *cursor = text;
    while (count < MAX_SCORES &&
           sscanf(cursor, "%15s %d%n", list[count].name, &list[count].score, &consumed) == 2)
    {
        cursor += consumed;
        count++;
    }

    UnloadFileText(text);
    return count;
}

void addHighScore(const char *playerName, int score)
{
    ScoreEntry list[MAX_SCORES];
    int count = loadHighScores(list);


    int slot = 0;
    while (slot < count && score <= list[slot].score)
        slot++;

    if (slot >= MAX_SCORES)
        return;

    for (int i = MAX_SCORES - 1; i > slot; i--)
        list[i] = list[i - 1];

    snprintf(list[slot].name, MAX_NAME, "%s", playerName);
    list[slot].score = score;
    if (count < MAX_SCORES)
        count++;


    char buffer[MAX_SCORES * (MAX_NAME + 16)];
    size_t used = 0;
    for (int i = 0; i < count; i++)
    {
        int written = snprintf(buffer + used, sizeof(buffer) - used,
                               "%s %d\n", list[i].name, list[i].score);
        if (written < 0 || (size_t)written >= sizeof(buffer) - used)
            return;
        used += (size_t)written;
    }

    SaveFileText(SAVE_DIR "/highscores.txt", buffer);
}

void clearHighScore(void)
{
    if (FileExists(SAVE_DIR "/highscores.txt"))
        remove(SAVE_DIR "/highscores.txt");
}

int loadLevelHighScores(const char *playerName, int scores[TOTAL_LEVELS])
{
    for (int i = 0; i < TOTAL_LEVELS; i++)
        scores[i] = 0;

    if (playerName == NULL || playerName[0] == '\0')
        return 0;

    const char *path = levelScoresFile(playerName);
    if (!FileExists(path))
        return 0;

    char *text = LoadFileText(path);
    if (text == NULL)
        return 0;

    int count = 0;
    const char *cursor = text;
    while (count < TOTAL_LEVELS)
    {
        int value = 0, consumed = 0;
        if (sscanf(cursor, "%d%n", &value, &consumed) != 1)
            break;

        scores[count++] = (value > 0) ? value : 0;
        cursor += consumed;
    }

    UnloadFileText(text);
    return count;
}

int saveLevelHighScore(const char *playerName, int level, int score)
{
    if (playerName == NULL || playerName[0] == '\0' ||
        level < 1 || level > TOTAL_LEVELS || score <= 0)
        return 0;

    int scores[TOTAL_LEVELS];
    loadLevelHighScores(playerName, scores);

    int index = level - 1;
    if (score <= scores[index])
        return 0;

    scores[index] = score;

    char buffer[TOTAL_LEVELS * 16];
    size_t used = 0;
    for (int i = 0; i < TOTAL_LEVELS; i++)
    {
        int written = snprintf(buffer + used, sizeof(buffer) - used, "%d%c",
                               scores[i], (i == TOTAL_LEVELS - 1) ? '\n' : ' ');
        if (written < 0 || (size_t)written >= sizeof(buffer) - used)
            return 0;
        used += (size_t)written;
    }

    return SaveFileText(levelScoresFile(playerName), buffer) ? 1 : 0;
}

void clearUnlockedLevel(const char *playerName)
{
    if (playerName == NULL || playerName[0] == '\0')
        return;

    const char *path = unlockFile(playerName);
    if (FileExists(path))
        remove(path);
}

void clearLevelHighScores(const char *playerName)
{
    if (playerName == NULL || playerName[0] == '\0')
        return;

    const char *path = levelScoresFile(playerName);
    if (FileExists(path))
        remove(path);
}


int loadAllPlayerScores(PlayerScores list[], int maxPlayers)
{
    if (list == NULL || maxPlayers <= 0)
        return 0;

    const char *prefix = "level_scores_";
    const size_t prefixLen = strlen(prefix);
    int count = 0;

    FilePathList files = LoadDirectoryFilesEx(SAVE_DIR, ".txt", false);

    for (unsigned int i = 0; i < files.count && count < maxPlayers; i++)
    {

        const char *base = GetFileNameWithoutExt(files.paths[i]);
        if (base == NULL || strncmp(base, prefix, prefixLen) != 0)
            continue;

        const char *who = base + prefixLen;
        if (who[0] == '\0')
            continue;

        PlayerScores *p = &list[count];
        snprintf(p->name, MAX_NAME, "%s", who);

        loadLevelHighScores(p->name, p->levels);

        p->total = 0;
        for (int l = 0; l < TOTAL_LEVELS; l++)
            p->total += p->levels[l];

        count++;
    }

    UnloadDirectoryFiles(files);


    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            if (list[j].total > list[i].total)
            {
                PlayerScores tmp = list[i];
                list[i] = list[j];
                list[j] = tmp;
            }
        }
    }

    return count;
}

void saveSettings(int soundOn, int mouseControl, int difficulty, float ballSpeed, float paddleSpeed, int startLives)
{
    SaveFileText(SAVE_DIR "/settings.txt",
                 (char *)TextFormat("%d %d %d %.2f %.2f %d",
                                    soundOn, mouseControl, difficulty, ballSpeed, paddleSpeed, startLives));
}

void loadSettings(int *soundOn, int *mouseControl, int *difficulty, float *ballSpeed, float *paddleSpeed, int *startLives)
{
    if (!FileExists(SAVE_DIR "/settings.txt"))
        return;

    char *text = LoadFileText(SAVE_DIR "/settings.txt");
    if (text == NULL)
        return;

    int savedSound = 1, savedMouse = 0, savedDifficulty = 1, savedStartLives = 3;
    float savedBallSpeed = 1.0f, savedPaddleSpeed = 7.0f;
    int parsed = sscanf(text, "%d %d %d %f %f %d",
                        &savedSound, &savedMouse, &savedDifficulty,
                        &savedBallSpeed, &savedPaddleSpeed, &savedStartLives);
    UnloadFileText(text);

    if (parsed != 6)
        return;


    if (!isfinite(savedBallSpeed))
        savedBallSpeed = 1.0f;
    if (!isfinite(savedPaddleSpeed))
        savedPaddleSpeed = 7.0f;

    if (savedBallSpeed < 0.5f)
        savedBallSpeed = 0.5f;
    if (savedBallSpeed > 2.0f)
        savedBallSpeed = 2.0f;
    if (savedPaddleSpeed < 3.0f)
        savedPaddleSpeed = 3.0f;
    if (savedPaddleSpeed > 14.0f)
        savedPaddleSpeed = 14.0f;

    *soundOn = (savedSound != 0);
    *mouseControl = (savedMouse != 0);
    *difficulty = (savedDifficulty < 0 || savedDifficulty > 2) ? 1 : savedDifficulty;
    *ballSpeed = savedBallSpeed;
    *paddleSpeed = savedPaddleSpeed;
    *startLives = (savedStartLives < 1 || savedStartLives > 5) ? 3 : savedStartLives;
}
