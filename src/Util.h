#ifndef D12_DRAW_UTIL_H
#define D12_DRAW_UTIL_H
#include <cstdlib>
#include <cstring>

inline void DebugLog(const char* fmt, ...) {
  char buf[1024];

  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  OutputDebugStringA(buf);
}

inline const wchar_t* GetWC(const char *c)
{
  const size_t cSize = strlen(c)+1;
  auto* wc = new wchar_t[cSize];
  mbstowcs (wc, c, cSize);

  return wc;
}

#endif // D12_DRAW_UTIL_H
