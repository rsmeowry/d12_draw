#ifndef D12_DRAW_GAMEWINDOW_H
#define D12_DRAW_GAMEWINDOW_H

#include <vector>
#include <windows.h>

class InputDevice;
class DXFramework;
class D3DApp;

class GameWindow {
  static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
  HWND handle = nullptr;

public:
  bool Init(HINSTANCE hInstance, int show, int w, int h, const wchar_t *title, D3DApp *gamePtr);
  [[nodiscard]] HWND GetHandle() const { return handle; }
  bool Update() const;
  static InputDevice *GetInputDevice();
};


#endif // D12_DRAW_GAMEWINDOW_H
