GAME_CORE_LIB = ./game_core
CORE_FOLDER=../game/core
SRC_FOLDER=src

ifeq ($(OS),Windows_NT)
    EXE_EXT = .exe
    LIB_DIR = win
    LIB_EXT = dll
    LDFLAGS = -lgdi32 -lole32 -ld3d11 -ldxgi
    RM = del /Q
    MKDIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
    RUN = $(BUILD_DIR)/$(TARGET).exe
else ifeq ($(shell uname -s),Darwin)
    EXE_EXT =
    LIB_DIR = mac
    LIB_EXT = dylib
    LDFLAGS = -framework Cocoa -framework QuartzCore -framework Metal -framework MetalKit
    RM = rm -f
    MKDIR = mkdir -p $(BUILD_DIR)
    RUN = ./$(BUILD_DIR)/$(TARGET)
else
    EXE_EXT =
    LIB_DIR = linux
    LIB_EXT = so
    LDFLAGS = -lGL -lX11 -lXi -lXcursor -lasound -lpthread -lm -ldl
    RM = rm -f
    MKDIR = mkdir -p $(BUILD_DIR)
    RUN = ./$(BUILD_DIR)/$(TARGET)
endif

FLAGS = -Wall -Wextra
TARGET = app
BUILD_DIR = build

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

GAME_CORE_PATH = $(GAME_CORE_LIB)/$(LIB_DIR)
GAME_CORE_LINK = -L$(GAME_CORE_PATH) -lgame_core

INCLUDE = -I$(CORE_FOLDER) -I$(SRC_FOLDER)

main:
	$(MKDIR)
	zig cc $(SOURCES) $(INCLUDE) -o $(BUILD_DIR)/$(TARGET)$(EXE_EXT) $(FLAGS) $(GAME_CORE_LINK) $(LDFLAGS)

run: main
	$(RUN)

clean:
	$(RM) $(BUILD_DIR)/*

.PHONY: main run clean