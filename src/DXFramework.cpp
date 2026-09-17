#include "DXFramework.h"

#include <cstdio>
#include <format>

#include "InputDevice.h"
#include "Util.h"

DXFramework::DXFramework() { time = Timer(); }

void DXFramework::Update() {
  time.Tick();
  gd.Update(time.DeltaTime());
}
