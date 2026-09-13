#include <iostream>
#include <windows.h>

#include "Util.h"
#include "Window.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
  Window win;

  if (!win.Init(hInstance, nShowCmd, 800, 640, "my d12 renderer :D"))
    return 0;

  MSG msg = { };
  while (msg.message != WM_QUIT) {
    win.Update();
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return 0;
}
