#!/bin/sh
set -eu

cd "$(dirname "$0")"

build_only=0
if [ "${1:-}" = "--build-only" ]; then
  build_only=1
  shift
fi

if [ "$#" -ne 0 ]; then
  echo "Usage: ./run.sh [--build-only]" >&2
  exit 2
fi

if ! command -v clang >/dev/null 2>&1; then
  echo "Error: clang is required. Install the Xcode Command Line Tools with: xcode-select --install" >&2
  exit 1
fi

if ! command -v brew >/dev/null 2>&1; then
  echo "Error: Homebrew is required. Install it from https://brew.sh, then run: brew install raylib" >&2
  exit 1
fi

if ! raylib_prefix=$(brew --prefix raylib 2>/dev/null); then
  echo "Error: raylib is not installed. Run: brew install raylib" >&2
  exit 1
fi

clang -Wall -std=c99 \
  src/main.c \
  src/render.c \
  src/assets.c \
  src/audio.c \
  src/game.c \
  src/gameplay.c \
  src/input.c \
  src/menus.c \
  src/hud.c \
  src/bricks.c \
  src/levels.c \
  src/ball.c \
  src/powerups.c \
  src/storage.c \
  -o dxball \
  -I"$raylib_prefix/include" \
  -L"$raylib_prefix/lib" \
  -lraylib \
  -framework CoreVideo \
  -framework IOKit \
  -framework Cocoa \
  -framework OpenGL

if [ "$build_only" -eq 1 ]; then
  exit 0
fi

exec ./dxball
