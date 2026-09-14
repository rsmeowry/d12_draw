
#ifndef D12_DRAW_D3DAPP_H
#define D12_DRAW_D3DAPP_H
#include "DXFramework.h"


class D3DApp {
private:
  bool mustExit = false;
protected:
  HINSTANCE hInstance;

  D3DApp(HINSTANCE): hInstance(hInstance) {};
  D3DApp(const D3DApp&) = delete;
  D3DApp& operator=(const D3DApp&) = delete;
  virtual ~D3DApp() { }

  virtual bool ExitRequested() = 0;
  virtual void OnResize() = 0;
  virtual bool Initialize() = 0;
  virtual void Update(const Timer& t) = 0;
  virtual void Draw(GraphicsDevice& gd, const Timer& t) = 0;

  virtual void OnMouseDown(WPARAM btnState, int x, int y) { };
  virtual void OnMouseUp(WPARAM btnState, int x, int y) { };
  virtual void OnMouseMove(WPARAM btnState, int x, int y) { };

public:
  void HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
  int Run();
private:
};


#endif // D12_DRAW_D3DAPP_H
