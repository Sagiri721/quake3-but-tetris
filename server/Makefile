CC = zig cc
CFLAGS = -O2
INCLUDES = -I../game/core

SERVER = netris_server
CORE_LIB = game_core/linux/libgame_core.so

all: $(SERVER)

$(SERVER): main.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(SERVER) main.c \
	$(CORE_LIB) \
	-Wl,-rpath,'$$ORIGIN/game_core/linux'

run: $(SERVER)
	./$(SERVER)

clean:
	rm -f $(SERVER)

.PHONY: all clean run