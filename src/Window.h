#include <vector>
#include <windows.h>

#ifndef D12_DRAW_WINDOW_H
#define D12_DRAW_WINDOW_H


class Window {
  static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
  HWND handle;

public:
  bool Init(HINSTANCE hInstance, int show, int w, int h, const char* title);
  HWND GetHandle() const { return handle; }
  bool Update();

};


#endif // D12_DRAW_WINDOW_H
