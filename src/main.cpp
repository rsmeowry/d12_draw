#include <iostream>
#include <windows.h>

#include "DXFramework.h"
#include "GameWindow.h"
#include "MyApp.h"
#include "Util.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
  MyApp app(hInstance);
  return app.Run();
}
