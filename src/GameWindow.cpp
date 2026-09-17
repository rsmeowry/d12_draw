#include "GameWindow.h"

#include <cstdio>

#include <windowsx.h>
#include "D3DApp.h"
#include "DXFramework.h"
#include "InputDevice.h"
#include "Util.h"

InputDevice *input = nullptr;

bool GameWindow::Init(HINSTANCE hInstance, int show, int w, int h, const wchar_t *title, D3DApp *gamePtr) {
  WNDCLASSEX wc = {};
  wc.cbSize = sizeof(wc);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName = nullptr;
  wc.lpszClassName = L"D12Draw";
  wc.hIconSm = wc.hIcon;

  if (!RegisterClassEx(&wc))
    return false;

  RECT windowRect = {0, 0, w, h};
  AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

  this->handle = CreateWindowEx(WS_EX_APPWINDOW, L"D12Draw", title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr,
                                nullptr, hInstance, gamePtr);

  input = new InputDevice(this->handle);

  ShowWindow(this->handle, SW_SHOW);
  UpdateWindow(this->handle);

  return true;
}

bool GameWindow::Update() { return UpdateWindow(this->handle); }

InputDevice *GameWindow::GetInputDevice() { return input; }

LRESULT GameWindow::WndProc(HWND hWnd, UINT umsg, WPARAM wprm, LPARAM lprm) {
  if (umsg == WM_CREATE) {
    const auto *const createStruct = reinterpret_cast<CREATESTRUCT *>(lprm);
    ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
  }

  switch (umsg) {
    case WM_DESTROY:
    case WM_CLOSE: {
      const LONG_PTR lptr = ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
      D3DApp *gamePtr = reinterpret_cast<D3DApp *>(lptr);
      gamePtr->HandleMessage(umsg, wprm, lprm);
      PostQuitMessage(0);
      return 0;
    }
    case WM_KEYDOWN: {
      const LONG_PTR lptr = ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
      D3DApp *gamePtr = reinterpret_cast<D3DApp *>(lptr);
      gamePtr->HandleMessage(umsg, wprm, lprm);
      return 0;
    }
    case WM_ACTIVATE: {
      const LONG_PTR lptr = ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
      D3DApp *gamePtr = reinterpret_cast<D3DApp *>(lptr);
      if (LOWORD(wprm) == WA_INACTIVE) {
        gamePtr->Deactivate();
      } else {
        gamePtr->Activate();
      }
      return 0;
    }
    case WM_ENTERSIZEMOVE: {
      const LONG_PTR lptr = ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
      D3DApp *gamePtr = reinterpret_cast<D3DApp *>(lptr);
      gamePtr->BeginResizing();
      return 0;
    }
    case WM_EXITSIZEMOVE: {
      const LONG_PTR lptr = ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
      D3DApp *gamePtr = reinterpret_cast<D3DApp *>(lptr);
      gamePtr->EndResizing();
      return 0;
    }
    case WM_MENUCHAR: {
      return MAKELRESULT(0, MNC_CLOSE);
    }
    case WM_GETMINMAXINFO: {
      ((MINMAXINFO *) lprm)->ptMinTrackSize.x = 200;
      ((MINMAXINFO *) lprm)->ptMinTrackSize.y = 200;
      return 0;
    }
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN: {
      const LONG_PTR lptr = ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
      D3DApp *gamePtr = reinterpret_cast<D3DApp *>(lptr);
      gamePtr->OnMouseDown(wprm, GET_X_LPARAM(lprm), GET_Y_LPARAM(lprm));
      return 0;
    }
    case WM_MOUSEMOVE: {
      const LONG_PTR lptr = ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
      D3DApp *gamePtr = reinterpret_cast<D3DApp *>(lptr);
      gamePtr->OnMouseMove(wprm, GET_X_LPARAM(lprm), GET_Y_LPARAM(lprm));
      return 0;
    }
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP: {
      const LONG_PTR lptr = ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
      D3DApp *gamePtr = reinterpret_cast<D3DApp *>(lptr);
      gamePtr->OnMouseUp(wprm, GET_X_LPARAM(lprm), GET_Y_LPARAM(lprm));
      return 0;
    }
    case WM_INPUT: {
      UINT dwSize = 0;
      GetRawInputData(reinterpret_cast<HRAWINPUT>(lprm), RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
      LPBYTE lpb = new BYTE[dwSize];
      if (lpb == nullptr) {
        return 0;
      }

      if (GetRawInputData((HRAWINPUT) lprm, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
        DebugLog("GetRawInputData does not return correct size !\n");

      RAWINPUT *raw = reinterpret_cast<RAWINPUT *>(lpb);

      if (raw->header.dwType == RIM_TYPEKEYBOARD) {
        input->OnKeyDown({raw->data.keyboard.MakeCode, raw->data.keyboard.Flags, raw->data.keyboard.VKey,
                          raw->data.keyboard.Message});
      } else if (raw->header.dwType == RIM_TYPEMOUSE) {
        input->OnMouseMove(
            {raw->data.mouse.usFlags, raw->data.mouse.usButtonFlags,
             static_cast<int>(raw->data.mouse.ulExtraInformation), static_cast<int>(raw->data.mouse.ulRawButtons),
             static_cast<short>(raw->data.mouse.usButtonData), raw->data.mouse.lLastX, raw->data.mouse.lLastY});
      }

      delete[] lpb;
      return DefWindowProc(hWnd, umsg, wprm, lprm);
    }
    default:
      return DefWindowProc(hWnd, umsg, wprm, lprm);
  }
}
