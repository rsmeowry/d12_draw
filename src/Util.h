#ifndef D12_DRAW_UTIL_H
#define D12_DRAW_UTIL_H
#include <cstdlib>
#include <cstring>

inline const wchar_t* GetWC(const char *c)
{
  const size_t cSize = strlen(c)+1;
  auto* wc = new wchar_t[cSize];
  mbstowcs (wc, c, cSize);

  return wc;
}

#endif // D12_DRAW_UTIL_H
