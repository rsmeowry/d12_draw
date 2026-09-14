#include "MyApp.h"

#include <format>

void MyApp::UpdateTitle() const {
  float dt = GetDX()->GetTime().DeltaTime();
  float fps = roundf(1. / dt);
  std::string title = std::format("my renderer :D | {} FPS", fps);
  SetWindowTextA(GetWin()->GetHandle(), title.c_str());
}
void MyApp::OnResize() {}
bool MyApp::Initialize() {
  OutputDebugStringA("MyApp::Initialize()");
  return true;
}
void MyApp::Update(const Timer &t) {
  UpdateTitle();
}
void MyApp::Draw(GraphicsDevice &gd, const Timer &t) {
  float col[4] = { static_cast<float>(abs(cos(t.GameTime() * 0.3))), 0.5, 0.2, 0.6 };
  gd.Clear(col);
}
