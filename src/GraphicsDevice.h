

#ifndef D12_DRAW_GRAPHICSDEVICE_H
#define D12_DRAW_GRAPHICSDEVICE_H

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class GraphicsDevice {
public:
  bool InitDevice();

  ComPtr<IDXGIFactory6> dxgiFactory;
  ComPtr<ID3D12Device> d3dDevice;
  ComPtr<ID3D12Fence> fence;

  UINT64 currentFence = 0;
  HANDLE fenceEvent = nullptr;

  UINT rtvDescriptorSize = 0;
  UINT dsvDescriptorSize = 0;
  UINT cbvSrvUavDescriptorSize = 0;
};

#endif // D12_DRAW_GRAPHICSDEVICE_H
