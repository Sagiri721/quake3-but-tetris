# The core game
The core game is the actual game logic, that is shared between the client and the server

It represents game state, the server must hold an imperative game state of what is going on, and the client references a copy of that state so that it can display it
It also handles the input queue, it defines what inputs are possible and how and when they were queued by the client/server

## Notes
**01/03/2026**: Removed input provider from tetris_board struct
An input provider pumps to the input queue asbtracting exactly how the input is registered, that way the client can define how (cpu, keyboard, network) and still populate the queue. It should be seperate from the game, it doesnt really matter, but I prefer it that way, since the game doesn't really need to know how it gets input, all it needs to know is that input is being registered.