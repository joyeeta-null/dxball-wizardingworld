@echo off
setlocal

cd /d "%~dp0"

set "BUILD_ONLY="
if /i "%~1"=="--build-only" (
  set "BUILD_ONLY=1"
  shift
)

if not "%~1"=="" (
  echo Usage: build.bat [--build-only]
  exit /b 2
)

where gcc >nul 2>nul
if errorlevel 1 (
  echo Error: gcc was not found on PATH.
  echo Install MinGW-w64 ^(https://www.mingw-w64.org, or MSYS2: pacman -S mingw-w64-x86_64-gcc^)
  echo and reopen the terminal so gcc.exe is visible.
  exit /b 1
)

if not exist "raylib\lib\libraylib.a" (
  echo Error: raylib\lib\libraylib.a is missing.
  echo The bundled raylib is required for the Windows build.
  exit /b 1
)

gcc -Wall -std=c99 ^
  src\main.c ^
  src\render.c ^
  src\assets.c ^
  src\audio.c ^
  src\game.c ^
  src\gameplay.c ^
  src\input.c ^
  src\menus.c ^
  src\hud.c ^
  src\bricks.c ^
  src\levels.c ^
  src\ball.c ^
  src\powerups.c ^
  src\storage.c ^
  -o dxball.exe ^
  -I raylib\include ^
  -L raylib\lib ^
  -lraylib ^
  -lopengl32 ^
  -lgdi32 ^
  -lwinmm ^
  -static

if errorlevel 1 (
  echo.
  echo Build failed.
  exit /b 1
)

if defined BUILD_ONLY exit /b 0

dxball.exe
