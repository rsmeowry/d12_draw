#ifndef D12_DRAW_UTIL_H
#define D12_DRAW_UTIL_H

#include <comdef.h>
#include <d3dcompiler.h>
#include <stdexcept>

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

inline ComPtr<ID3D12Resource> CreateDefaultBuffer(
  ID3D12Device* device,
  ID3D12GraphicsCommandList* cmdList,
  const void* initData,
  UINT64 byteSize,
  ComPtr<ID3D12Resource>& uploadBuffer)
{
  ComPtr<ID3D12Resource> defaultBuffer;

  CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
  CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

  ThrowIfFailed(device->CreateCommittedResource(
    &defaultHeapProps,
    D3D12_HEAP_FLAG_NONE,
    &bufferDesc,
    D3D12_RESOURCE_STATE_COMMON,
    nullptr,
    IID_PPV_ARGS(&defaultBuffer)));

  CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);

  ThrowIfFailed(device->CreateCommittedResource(
    &uploadHeapProps,
    D3D12_HEAP_FLAG_NONE,
    &bufferDesc,
    D3D12_RESOURCE_STATE_GENERIC_READ,
    nullptr,
    IID_PPV_ARGS(&uploadBuffer)));

  D3D12_SUBRESOURCE_DATA subResourceData = {};
  subResourceData.pData = initData;
  subResourceData.RowPitch = byteSize;
  subResourceData.SlicePitch = byteSize;

  CD3DX12_RESOURCE_BARRIER toCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(
    defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
  cmdList->ResourceBarrier(1, &toCopyDest);

  UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

  CD3DX12_RESOURCE_BARRIER toGenericRead = CD3DX12_RESOURCE_BARRIER::Transition(
    defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
  cmdList->ResourceBarrier(1, &toGenericRead);

  return defaultBuffer;
}

inline ComPtr<ID3DBlob> CompileShader(
  const std::wstring& filename,
  const D3D_SHADER_MACRO* defines,
  const std::string& entrypoint,
  const std::string& target)
{
  UINT compileFlags = 0;
#if defined(DX12_ENABLE_DEBUG_LAYER)
  compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

  ComPtr<ID3DBlob> byteCode;
  ComPtr<ID3DBlob> errors;

  HRESULT hr = D3DCompileFromFile(
    filename.c_str(),
    defines,
    D3D_COMPILE_STANDARD_FILE_INCLUDE,
    entrypoint.c_str(),
    target.c_str(),
    compileFlags,
    0,
    &byteCode,
    &errors);

  if (errors != nullptr)
  {
    OutputDebugStringA(static_cast<const char*>(errors->GetBufferPointer()));
  }

  ThrowIfFailed(hr);

  return byteCode;
}

#endif // D12_DRAW_UTIL_H