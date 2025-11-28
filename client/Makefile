ifeq ($(OS),Windows_NT)
    LDFLAGS = -lgdi32 -lole32 -ld3d11 -ldxgi
else ifeq ($(shell uname -s),Darwin)
    LDFLAGS = -framework Cocoa -framework QuartzCore -framework Metal -framework MetalKit
else
    LDFLAGS = -lGL -lX11 -lXi -lXcursor -lpthread -lm -ldl
endif

TARGET = app

SOURCES = \
    src/main.c \
	src/game.c \
    src/core/tetris.c \
	src/core/input.c \
    src/gfx/render.c 

main:
	zig cc $(SOURCES) -o build/$(TARGET).exe $(LDFLAGS)

run: main
	./build/app.exe