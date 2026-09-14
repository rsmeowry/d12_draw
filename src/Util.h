#ifndef D12_DRAW_UTIL_H
#define D12_DRAW_UTIL_H
#include <comdef.h>
#include <cstdlib>
#include <stdexcept>
#include <cstring>

inline std::string WStringToString(const std::wstring& wstr)
{
  if (wstr.empty()) return {};

  int size = WideCharToMultiByte(
    CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()),
    nullptr, 0, nullptr, nullptr);

  std::string result(size, '\0');
  WideCharToMultiByte(
    CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()),
    result.data(), size, nullptr, nullptr);

  return result;
}

inline void ThrowIfFailed(HRESULT hr)
{
  if (FAILED(hr))
  {
    _com_error err(hr);
    throw std::runtime_error(WStringToString(err.ErrorMessage()));
  }
}

inline void DebugLog(const char* fmt, ...) {
  char buf[1024];

  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  OutputDebugStringA(buf);
}

#endif // D12_DRAW_UTIL_H
