#include "Game.h"

#include <cstdio>
#include <format>

#include "InputDevice.h"
#include "Util.h"
void Game::HandleWindowMessage(UINT msg, WPARAM wprm, LPARAM lprm) {
  DebugLog("MESSAGE %04i %i %i", msg, wprm, lprm);

  if (msg == WM_DESTROY || msg == WM_CLOSE || msg == WM_QUIT) {
    exitRequested = true;
  }
}

void Game::Run(GameWindow win) {
  MSG msg = { };
  while (!exitRequested) {
    exitRequested = msg.message == WM_QUIT;
    win.Update();
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);

      if (win.GetInputDevice()->IsKeyDown(Keys::A)) {
        DebugLog("KEY A PRESSED");
      }
    }
  }
}
