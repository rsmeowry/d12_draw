
#include "MyApp.h"

void MyApp::OnResize() {}
bool MyApp::Initialize() {
  OutputDebugStringA("MyApp::Initialize()");
  return true;
}
void MyApp::Update(const Timer &t) {

}
void MyApp::Draw(GraphicsDevice &gd, const Timer &t) {
  float col[4] = { static_cast<float>(abs(cos(t.GameTime() * 0.3))), 0.5, 0.2, 0.6 };
  gd.Clear(col);
}
