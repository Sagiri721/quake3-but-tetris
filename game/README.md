# The core game
The core game is the actual game logic, that is shared between the client and the server

It represents game state, the server must hold an imperative game state of what is going on, and the client references a copy of that state so that it can display it
It also handles the input queue, it defines what inputs are possible and how and when they were queued by the client/server