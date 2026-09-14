#include <windows.h>

#include "D3DApp.h"

Timer::Timer() {
  LARGE_INTEGER perSec;
  QueryPerformanceFrequency(&perSec);
  secondsPerCount = 1. / static_cast<double>(perSec.QuadPart);
}

float Timer::GameTime() const {
  if (stopped)
    return static_cast<float>((stopTime - pausedTime) - baseTime) * secondsPerCount;

  return static_cast<float>((currTime - pausedTime) - baseTime) * secondsPerCount;
}
float Timer::DeltaTime() const {
  return static_cast<float>(deltaTime);
}

void Timer::Reset() {
  __int64 cTime;
  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&cTime));

  baseTime = cTime;
  prevTime = cTime;
  stopTime = 0;
  stopped = false;
}

void Timer::Start() {
  if (!stopped)
    return;

  __int64 startTime;
  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&startTime));
  pausedTime += (startTime - stopTime);
  prevTime = startTime;
  stopTime = 0;
  stopped = false;
}

void Timer::Stop() {
  if (stopped)
    return;

  __int64 cTime;
  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&cTime));
  stopTime = cTime;
  stopped = true;
}

void Timer::Tick() {
  if (stopped) {
    deltaTime = 0.;
    return;
  }

  __int64 cTime;
  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&cTime));
  currTime = cTime;

  deltaTime = (currTime - prevTime) * secondsPerCount;
  prevTime = currTime;

  if (deltaTime < 0.)
    deltaTime = 0.;
}