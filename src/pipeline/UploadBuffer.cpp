#include "UploadBuffer.h"

#include "Util.h"
#include "d3dx12_core.h"

template<typename T>
UploadBuffer<T>::UploadBuffer(ID3D12Device *device, UINT elementCount, bool iisConstantBuffer) : isConstantBuffer(iisConstantBuffer) {
  elementByteSize = sizeof(T);

  if (isConstantBuffer) {
    elementByteSize = CalcConstantBufferByteSize(elementByteSize);
  }

  auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
  auto buf = CD3DX12_RESOURCE_DESC::Buffer(elementByteSize*elementCount);
  ThrowIfFailed(device->CreateCommittedResource(
    &prop,
    D3D12_HEAP_FLAG_NONE,
    &buf,
    D3D12_RESOURCE_STATE_GENERIC_READ,
    nullptr,
    IID_PPV_ARGS(&uploadBuffer)));

  ThrowIfFailed(uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData)));
}

template<typename T>
UploadBuffer<T>::~UploadBuffer() {
  if (uploadBuffer != nullptr)
    uploadBuffer->Unmap(0, nullptr);

  mappedData = nullptr;
}