#include "Window.h"

#include "Util.h"

bool Window::Init(HINSTANCE hInstance, int show, int w, int h, const char *title) {
  WNDCLASSEX wc = { };
  wc.cbSize = sizeof(wc);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = Window::WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName = nullptr;
  wc.lpszClassName = "D12Draw";
  wc.hIconSm = wc.hIcon;

  if (!RegisterClassEx(&wc))
    return false;

  RECT windowRect = { 0, 0, w, h };
  AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

  this->handle = CreateWindowEx(
    WS_EX_APPWINDOW,
    "D12Draw",
    title,
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    windowRect.right - windowRect.left,
    windowRect.bottom - windowRect.top,
    nullptr,
    nullptr,
    hInstance,
    nullptr
    );

  ShowWindow(this->handle, SW_SHOW);
  UpdateWindow(this->handle);

  return true;
}
bool Window::Update() {
  return UpdateWindow(this->handle);
}

LRESULT Window::WndProc(HWND hWnd, UINT umsg, WPARAM wprm, LPARAM lprm) {
  if (umsg == WM_CREATE) {
    const auto* const createStruct = reinterpret_cast<CREATESTRUCT*>(lprm);
    ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
  }

  switch (umsg) {
    case WM_DESTROY:
    case WM_CLOSE:
    {
      PostQuitMessage(0);
      return 0;
    }
    default: {
      return DefWindowProc(hWnd, umsg, wprm, lprm);
    }
  }
}