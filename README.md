# Made tetris again award
![Windows](https://custom-icon-badges.demolab.com/badge/Windows-0078D6?logo=windows11&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat&logo=linux&logoColor=black)
![macOS](https://img.shields.io/badge/macOS-000000?logo=apple&logoColor=F0F0F0)

A tetris game in C with sokol gp inspired by Quake3 architecture (Mac build NOT tested !!!!)

## How to build
**Requirements:**
1. Zig compiler

**Steps:**

1. After cloning, run `git submodule update --init --recursive` to include [sokol_gp](https://github.com/edubart/sokol_gp)
2. Add [sokol_audio.h](https://github.com/floooh/sokol/blob/master/sokol_audio.h) to `client\src\sokol_gp\thirdparty`
2. (if needed) You may need to download graphics backends (X11, Opengl, et al.) and audio drivers (asla)
3. `cd game` ; `make`
4. `cd client` ; `make`
5. `.\client\build\app.exe` || `.\client\build\app`

**Note**: Makefiles are platform independent (not tested on mac)

## Features
- Seperate game core and client implementation (dynamic linking)
- Quake3 style input queue
- Custom bitmap font renderer (on top of stb)
- Custom ogg player (on top of stb)
- Menu stack
- Multi threading (render & audio)
- Basic (but extensible) Tetris AI and input providing

### Goals
1. Quake3 style server-client bots
2. Multiplayer via the network input channel