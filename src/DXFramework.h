#ifndef D12_DRAW_GAME_H
#define D12_DRAW_GAME_H

#include <windows.h>

#include "GameWindow.h"
#include "GraphicsDevice.h"
#include "Timer.h"

class InputDevice;

class DXFramework {
  bool exitRequested = false;
  bool paused = false;
  Timer time;
  GraphicsDevice gd;
  InputDevice* input = nullptr;
public:
  DXFramework();

  void Initialize(GameWindow win) {
    input = win.GetInputDevice();
  }

  void Update();

  Timer& GetTime() { return time; }
  GraphicsDevice& GetGD() { return gd; }
  InputDevice* GetInput() const { return input; }
};

#endif // D12_DRAW_GAME_H
