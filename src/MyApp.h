#ifndef D12_DRAW_MYAPP_H
#define D12_DRAW_MYAPP_H
#include "D3DApp.h"


class MyApp final : public D3DApp {
private:
  bool exitRequested = false;

  void UpdateTitle() const;

public:
  explicit MyApp(HINSTANCE hinstance) : D3DApp(hinstance) {}

protected:
  bool ExitRequested() override { return exitRequested; }
  void OnResize() override;
  bool Initialize() override;
  void Update(const Timer &t) override;
  void Draw(GraphicsDevice &gd, const Timer &t) override;
};


#endif // D12_DRAW_MYAPP_H
