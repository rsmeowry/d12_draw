#include <iostream>
#include <windows.h>

#include "DXFramework.h"
#include "GameWindow.h"
#include "Util.h"
#include "apps/BoxApp.h"
#include "apps/MyApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd) {
  // MyApp app(hInstance);
  // return app.Run();
  BoxApp app(hInstance);
  app.Run();
}
