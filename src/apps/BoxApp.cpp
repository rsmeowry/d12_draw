#include "BoxApp.h"

#include <format>

void BoxApp::UpdateTitle() const {
  const float dt = GetDX()->GetTime().DeltaTime();
  float fps = roundf(1. / dt);
  const std::string title = std::format("my renderer :D | {} FPS", fps);
  SetWindowTextA(GetWin()->GetHandle(), title.c_str());
}
void BoxApp::OnResize() {}
bool BoxApp::Initialize() {
  OutputDebugStringA("BoxApp::Initialize()");
  return true;
}
void BoxApp::Update(const Timer &t) { UpdateTitle(); }
void BoxApp::Draw(GraphicsDevice &gd, const Timer &t) {
  const float col[4] = {static_cast<float>(abs(cos(t.GameTime() * 0.3))), 0.5, 0.2, 0.6};
  gd.Clear(col);

  gd.RenderContainedObject();
}
