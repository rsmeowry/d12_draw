#ifndef D12_DRAW_TIME_H
#define D12_DRAW_TIME_H

class Timer {
  double secondsPerCount = 0;
  double deltaTime = -1.;

  // total time spent
  __int64 baseTime = 0;
  // total time spent in pause
  __int64 pausedTime = 0;
  // time we got paused
  __int64 stopTime = 0;
  // last tick time
  __int64 prevTime = 0;
  // current tick from the start
  __int64 currTime = 0;
  // is paused
  bool stopped = false;

public:
  Timer();

  float GameTime() const;
  float DeltaTime() const;

  void Reset();
  void Start();
  void Stop();
  void Tick();
};

#endif // D12_DRAW_TIME_H
