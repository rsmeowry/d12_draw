#ifndef D12_DRAW_GAME_H
#define D12_DRAW_GAME_H

#include "GameWindow.h"
#include <windows.h>

class Game {
  bool exitRequested = false;
public:
  void HandleWindowMessage(UINT, WPARAM, LPARAM);
  void Run(GameWindow win);
};

#endif // D12_DRAW_GAME_H
