
#include "D3DApp.h"

#include "InputDevice.h"

void D3DApp::HandleMessage(const UINT msg, WPARAM, LPARAM) {
  if (msg == WM_DESTROY || msg == WM_CLOSE || msg == WM_QUIT) {
    mustExit = true;
  }
}
int D3DApp::Run() {
  try {
    DXFramework fw;
    this->dx = &fw;

    GameWindow win;
    this->window = &win;

    if (!win.Init(hInstance, 0, 800, 640, L"My D3D Renderer :D", this))
      return 0;

    fw.Initialize(win);

    if (!Initialize())
      return -1;

    RECT rect;
    GetWindowRect(win.GetHandle(), &rect);
    fw.GetGD().Initialize(win.GetHandle(), rect.right - rect.left, rect.bottom - rect.top);

    MSG msg = {};
    fw.GetTime().Reset();
    while (!mustExit) {
      win.Update();
      while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }

      fw.Update();

      Update(fw.GetTime());

      fw.GetGD().PrepareRt();
      Draw(fw.GetGD(), fw.GetTime());
      fw.GetGD().Display();
    }

    return 0;
  } catch (const std::exception &e) {
    MessageBoxA(nullptr, e.what(), "Error in renderer :(", MB_OK | MB_ICONERROR);
    return -1;
  }
}
void D3DApp::Activate() {
  appPaused = true;
  dx->GetTime().Stop();
}
void D3DApp::Deactivate() {
  appPaused = false;
  dx->GetTime().Start();
}
void D3DApp::BeginResizing() {
  appPaused = true;
  resizing = true;
  dx->GetTime().Stop();
}
void D3DApp::EndResizing() {
  appPaused = false;
  resizing = false;
  dx->GetTime().Start();
  OnResize();
}
