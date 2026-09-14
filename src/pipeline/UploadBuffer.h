#ifndef D12_DRAW_UPLOADBUFFER_H
#define D12_DRAW_UPLOADBUFFER_H
#include <d3d12.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

template<typename T>
class UploadBuffer {
  ComPtr<ID3D12Resource> uploadBuffer;
  BYTE* mappedData = nullptr;
  UINT elementByteSize = 0;
  bool isConstantBuffer = false;

public:
  UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer);
  UploadBuffer(const UploadBuffer&) = delete;
  UploadBuffer& operator=(const UploadBuffer&) = delete;
  ~UploadBuffer();

  ID3D12Resource* GetBuffer() { return uploadBuffer.Get(); }
  void CopyData(int elementIdx, const T& data) {
    memcpy(&mappedData[elementIdx * elementByteSize], &data, sizeof(T));
  }
};

#endif // D12_DRAW_UPLOADBUFFER_H
