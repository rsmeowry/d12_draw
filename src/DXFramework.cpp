#include "DXFramework.h"

DXFramework::DXFramework() { time = Timer(); }

void DXFramework::Update() {
  time.Tick();
  gd.Update(time.DeltaTime());
}
