#include <iostream>
#include <windows.h>

#include "Game.h"
#include "GameWindow.h"
#include "Util.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
  Game game;
  GameWindow win;

  if (!win.Init(hInstance, nShowCmd, 800, 640, L"my d12 renderer :D", &game))
    return 0;

  game.Run(win);

  return 0;
}
