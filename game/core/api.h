#ifndef GAME_API_H
#define GAME_API_H

// Export symbols for dynamic linking for dlls/shared objects
#ifdef _WIN32
  #ifdef GAME_API_EXPORTS
    #define GAME_API __declspec(dllexport)
  #else
    #define GAME_API __declspec(dllimport)
  #endif
#else
    #define GAME_API __attribute__((visibility("default")))
#endif

#endif