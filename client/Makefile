GAME_CORE_LIB = ./game_core
CORE_FOLDER=../game/core

ifeq ($(OS),Windows_NT)
    LDFLAGS = -lgdi32 -lole32 -ld3d11 -ldxgi
	GAME_CORE_LINK = $(GAME_CORE_LIB)/win/libgame_core.dll
else ifeq ($(shell uname -s),Darwin)
    LDFLAGS = -framework Cocoa -framework QuartzCore -framework Metal -framework MetalKit
# Nope, not implemented yet
else
    LDFLAGS = -lGL -lX11 -lXi -lXcursor -lpthread -lm -ldl
	GAME_CORE_LINK = $(GAME_CORE_LIB)/linux/libgame_core.so
endif

FLAGS = -Wall -Wextra
TARGET = app

SOURCES = \
    src/main.c \
	src/game.c \
	src/input/input_table.c \
	src/input/providers/input_keyboard.c \
	src/input/providers/input_cpu.c \
    src/gfx/render.c \
	src/queue/queue.c \
	src/gfx/image.c \
	src/gfx/bitmap_text.c \
	src/bot/ai.c \
	src/bot/heuristics/greedy_score.c \
	src/bot/heuristics/weighted_score.c \
	src/bot/ai_utils.c \
	src/gfx/menu.c \
	src/audio/stb_vorbis.c \
	src/audio/ogg_player.c \
	src/lib/tinycthread.c \

main:
	zig cc $(SOURCES) -I$(CORE_FOLDER) -o build/$(TARGET).exe $(FLAGS) $(GAME_CORE_LINK) $(LDFLAGS)
	$(COPY_DLL)

run: main
	./build/app.exe

clean:
	del build\* /Q

.PHONY: main run clean build_dir