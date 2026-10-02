# DX Ball — Wizarding World

A wizarding-themed brick-breaker written in C99 with
[raylib](https://www.raylib.com/). The game includes seven levels, multiple
brick and power-up types, adjustable difficulty, keyboard or mouse paddle
control, resumable games, level selection, music, sound effects, and local
high-score tracking.

## Project team

- Ashifa J. Rahman — Student ID: 2505106
- Joyeeta Mitra — Student ID: 2505100

- **Department:** Computer Science and Engineering (CSE)
- **University:** Bangladesh University of Engineering and Technology (BUET)
- **Supervisor:** Anwarul Bashir Shuaib

> [!IMPORTANT]
> Build and run the game from the repository root. The game loads assets and
> save data with relative paths. The supplied scripts switch to the correct
> directory automatically.

## Dependencies

| Platform | Required software | Notes |
| --- | --- | --- |
| All builds | A C99 compiler and raylib | The bundled Windows files target raylib 5.5. The macOS build also compiles with raylib 6.0. |
| macOS | Xcode Command Line Tools, Homebrew, and the Homebrew `raylib` formula | `run.sh` links the Cocoa, OpenGL, IOKit, and CoreVideo system frameworks. |
| Windows | 64-bit MinGW-w64 GCC | The required raylib 5.5 headers and x86-64 static library are included in the repository. Visual C++ is not supported by the provided script. |

Beyond the platform dependencies listed above, no additional C libraries,
environment variables, asset downloads, or asset-generation steps are
required. All images, fonts, music, and sound effects used by the game are
included in `assets/`.

## Quick start

### macOS

Install the required developer tools and raylib:

```sh
xcode-select --install
brew install raylib
```

From the repository root, build and start the game:

```sh
./run.sh
```

The script finds Clang, Homebrew, and raylib, compiles the project to
`dxball`, and then starts it.

If macOS reports that `run.sh` is not executable, fix its permission once and
try again:

```sh
chmod +x run.sh
./run.sh
```

### Windows

1. Install a **64-bit MinGW-w64 GCC** toolchain.
2. Add the toolchain's `bin` directory to `PATH`.
3. Open Command Prompt or PowerShell in the repository root.
4. Double-click `build.bat`, or run the build script from a terminal:

   **Command Prompt**

   ```bat
   build.bat
   ```

   **PowerShell**

   ```powershell
   .\build.bat
   ```

The script builds `dxball.exe` and starts the game. The raylib headers and
64-bit MinGW static library required by the Windows build are already included
in `raylib\include\` and `raylib\lib\`.

## Compiling without immediately running

Both provided scripts accept `--build-only` as their first argument.

### macOS

```sh
./run.sh --build-only
./dxball
```

### Windows Command Prompt

```bat
build.bat --build-only
dxball.exe
```

### Windows PowerShell

```powershell
.\build.bat --build-only
.\dxball.exe
```

## Controls

| Context | Input | Action |
| --- | --- | --- |
| Menus | Left click | Select menu items and on-screen controls |
| Main menu | `Enter` | Start or continue player setup |
| Menus | `Esc` | Go back; from the main menu, quit |
| Name entry | Letters and numbers | Enter a player name of up to 12 characters |
| Name entry | `Backspace` / `Enter` | Delete a character / confirm the name |
| Level intro | `Enter` or left click | Begin the level |
| Gameplay | Left/Right arrow keys | Move the paddle |
| Gameplay | Mouse movement | Move the paddle when Mouse Control is enabled |
| Gameplay | `Space` or left click | Launch the ball |
| Gameplay | `P` or `Esc` | Pause or resume the game |
| Paused or game over | `R` | Restart the current level |
| End screen | `M` | Return to the main menu |
| Developer shortcut | `Shift` + `L` | Clear the current level |

The objective is to break every destructible brick without running out of
lives. Catch dropped power-ups to gain effects such as a wider paddle,
multiple balls, extra speed, invincibility, or an extra life.

## Configuration and saved data

Always launch the executable **from the repository root**. Asset and save-file
paths are relative to the current working directory:

```text
assets/...
saves/settings.txt
saves/highscores.txt
saves/progress_<PLAYER>.txt
saves/unlocked_<PLAYER>.txt
saves/level_scores_<PLAYER>.txt
```

For example, use `./dxball` while your terminal is in the repository root.
Starting the executable with another working directory will prevent the game
from finding its images, fonts, and audio, and may create a separate `saves/`
directory in the wrong location.

The `saves/` directory must be writable if you want progress and settings to
persist. It is created automatically when missing. The game uses these files:

- `saves/settings.txt` stores sound, mouse-control, difficulty, ball-speed,
  paddle-speed, and starting-life preferences.
- `saves/highscores.txt` stores the global high-score table.
- `saves/progress_<PLAYER>.txt` stores a resumable game, including the level,
  score, lives, and remaining brick state.
- `saves/unlocked_<PLAYER>.txt` stores the highest unlocked level for a
  player.
- `saves/level_scores_<PLAYER>.txt` stores that player's best score for each
  level.

No manual configuration is required for a first run. Sound, mouse control,
ball speed, paddle speed, and difficulty are changed through the in-game
Settings screen and saved automatically. Player names are converted to
uppercase and may contain up to 12 ASCII letters or numbers.

The Settings screen's reset action restores the default options, clears the
current player's saved progress, unlocked levels, and per-level scores, and
clears the global high-score table.

## Project layout

```text
.
├── assets/             Fonts, backgrounds, sprites, UI images, and audio
├── output/             Generated project documentation
├── raylib/
│   ├── include/        Bundled raylib 5.5 headers for Windows
│   └── lib/            Bundled 64-bit Windows raylib libraries
├── saves/              Player progress, unlocks, scores, and settings
├── src/                Game source and header files
├── build.bat           Windows build-and-run script
└── run.sh              macOS build-and-run script
```

Every current `.c` file in `src/` is listed in both build scripts. New source
files must be added to the compile command in `run.sh` and `build.bat`.

Key modules include:

- `src/main.c`: application startup, main loop, render target, and shutdown;
- `src/game.c` and `src/gameplay.c`: game state, transitions, and frame
  updates;
- `src/input.c`: keyboard and mouse input;
- `src/render.c`, `src/menus.c`, and `src/hud.c`: rendering and user
  interface;
- `src/bricks.c`, `src/ball.c`, `src/levels.c`, and `src/powerups.c`: core
  game systems;
- `src/assets.c` and `src/audio.c`: resource loading, playback, and cleanup;
  and
- `src/storage.c`: settings, progress, level unlocks, and score persistence.

## Troubleshooting

### `clang is required` on macOS

Install or repair the Xcode Command Line Tools:

```sh
xcode-select --install
```

### `Homebrew is required` or `raylib is not installed` on macOS

Install [Homebrew](https://brew.sh/) if necessary, then install raylib:

```sh
brew install raylib
```

### `gcc was not found on PATH` on Windows

Install a 64-bit MinGW-w64 GCC distribution and ensure its `bin` directory is
on `PATH`. Open a new terminal and verify it with:

```bat
gcc --version
```

### `raylib\lib\libraylib.a` is missing on Windows

Restore the bundled `raylib/` directory. The provided build script requires
`raylib\include\raylib.h` and `raylib\lib\libraylib.a`.

### The window opens but assets or audio are missing

Close the game, change to the repository root, and launch the executable from
there. The working directory must contain `assets/`; the game creates
`saves/` automatically when needed.

### Progress or settings do not persist

Check that your user account can write to the `saves/` directory. Also confirm
that the game is being launched from the repository root rather than another
working directory.

### The game has no sound

Check that Sound is enabled in Settings and that the operating system has an
active audio output device. Music and sound-effect files are loaded from
`assets/sounds/`.
