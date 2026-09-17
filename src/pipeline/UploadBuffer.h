#ifndef D12_DRAW_UPLOADBUFFER_H
#define D12_DRAW_UPLOADBUFFER_H
#include <d3d12.h>
#include <wrl/client.h>

#include "Util.h"

using Microsoft::WRL::ComPtr;

template<typename T>
class UploadBuffer {
  ComPtr<ID3D12Resource> uploadBuffer;
  BYTE *mappedData = nullptr;
  UINT elementByteSize = 0;
  bool isConstantBuffer = false;

public:
  UploadBuffer(ID3D12Device *device, UINT elementCount, bool sIsConstantBuffer) : isConstantBuffer(sIsConstantBuffer) {
    elementByteSize = sizeof(T);

    if (isConstantBuffer) {
      elementByteSize = CalcConstantBufferByteSize(elementByteSize);
    }

    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto buf = CD3DX12_RESOURCE_DESC::Buffer(elementByteSize * elementCount);
    ThrowIfFailed(device->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &buf, D3D12_RESOURCE_STATE_GENERIC_READ,
                                                  nullptr, IID_PPV_ARGS(&uploadBuffer)));

    ThrowIfFailed(uploadBuffer->Map(0, nullptr, reinterpret_cast<void **>(&mappedData)));
  }
  UploadBuffer(const UploadBuffer &) = delete;
  UploadBuffer &operator=(const UploadBuffer &) = delete;
  ~UploadBuffer() {
    if (uploadBuffer != nullptr)
      uploadBuffer->Unmap(0, nullptr);

    mappedData = nullptr;
  }

  UINT GetElementByteSize() const { return elementByteSize; }
  D3D12_GPU_VIRTUAL_ADDRESS GetElementAddress(UINT elementIdx) const {
    return uploadBuffer->GetGPUVirtualAddress() + elementIdx * elementByteSize;
  }
  ID3D12Resource *GetBuffer() const { return uploadBuffer.Get(); }
  void CopyData(int elementIdx, const T &data) { memcpy(&mappedData[elementIdx * elementByteSize], &data, sizeof(T)); }
};

#endif // D12_DRAW_UPLOADBUFFER_H
