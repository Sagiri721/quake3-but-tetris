CC = zig cc
CFLAGS = -O2 -fPIC

# Source files
SRCS = core\tetris.c core\input.c core\rng.c core\queue\queue.c

BUILD_DIR = build

# Output
LIB_WINDOWS = libgame_core.dll
LIB_LINUX = libgame_core.so

all: windows linux

windows: $(LIB_WINDOWS)
	xcopy $(BUILD_DIR)\win ..\client\game_core\win /s /e /y

linux: $(LIB_LINUX) 
	xcopy  $(BUILD_DIR)\linux ..\client\game_core\linux /s /e /y

$(LIB_LINUX): $(SRCS) $(BUILD_DIR)
	$(CC) $(CFLAGS) -shared -o $(BUILD_DIR)\linux\$(LIB_LINUX) $(SRCS)

$(LIB_WINDOWS): $(SRCS) $(BUILD_DIR)
	$(CC) $(CFLAGS) -shared -o $(BUILD_DIR)\win\$(LIB_WINDOWS) $(SRCS)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)\win
	mkdir $(BUILD_DIR)\linux

clean:
	rm -f $(LIB)

.PHONY: clean