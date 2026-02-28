CC = zig cc
CFLAGS = -O2 -fPIC

# Detect OS
ifeq ($(OS),Windows_NT)
    RM = rmdir /s /q
    MKDIR = mkdir
    CP = xcopy /e /i /y
    SEP = \\
    LIB_EXT = dll
    OUT_DIR = win
else
    RM = rm -rf
    MKDIR = mkdir -p
    CP = cp -r
    SEP = /
    LIB_EXT = so
    OUT_DIR = linux
endif

# Source files
SRCS = core/tetris.c core/input.c core/rng.c core/queue/queue.c

# Build
BUILD_DIR = build

LIB_NAME = libgame_core.$(LIB_EXT)

all: $(LIB_NAME)

$(LIB_NAME): $(SRCS)
	$(MKDIR) $(BUILD_DIR)/$(OUT_DIR)
	$(CC) $(CFLAGS) -shared -o $(BUILD_DIR)/$(OUT_DIR)/$(LIB_NAME) $(SRCS)
	$(CP) $(BUILD_DIR) ..$(SEP)client$(SEP)game_core

clean:
	$(RM) $(BUILD_DIR)

.PHONY: all clean
