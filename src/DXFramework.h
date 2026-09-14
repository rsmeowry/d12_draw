#ifndef D12_DRAW_GAME_H
#define D12_DRAW_GAME_H

#include "GameWindow.h"
#include <windows.h>

#include "GraphicsDevice.h"
#include "Timer.h"

class DXFramework {
  bool exitRequested = false;
  bool paused = false;
  Timer time;
  GraphicsDevice gd;
public:
  DXFramework();

  void HandleWindowMessage(UINT, WPARAM, LPARAM);
  void Run(GameWindow win);

  Timer* GetTime();
};

#endif // D12_DRAW_GAME_H
