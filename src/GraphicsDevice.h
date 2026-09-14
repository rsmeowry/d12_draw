

#ifndef D12_DRAW_GRAPHICSDEVICE_H
#define D12_DRAW_GRAPHICSDEVICE_H

#include <SimpleMath.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class GraphicsDevice {
private:
  bool InitDevice();
  void CreateFence();
  void CacheDescriptorSizes();
  void CreateCommandObjects();
  void CreateSwapChain(HWND hWnd, UINT width, UINT height);
  void CreateRtvHeap();
  void CreateRenderViews();
  void CreateDsvHeap();
  void CreateDepthStencilBuffer(UINT width, UINT height);
  void SetViewportAndScissor(UINT width, UINT height);
  void FlushCommandQueue();
  D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBbView() const;
  ID3D12Resource* GetCurrentBb() const;
  D3D12_CPU_DESCRIPTOR_HANDLE GetDsv() const;

  static constexpr UINT swapChainBufferCount = 2;
  UINT currBackBuffer = 0;

  ComPtr<IDXGIFactory6> dxgiFactory;
  ComPtr<ID3D12Device> d3dDevice;
  ComPtr<ID3D12Fence> fence;
  ComPtr<ID3D12CommandQueue> commandQueue;
  ComPtr<ID3D12CommandAllocator> commandAlloc;
  ComPtr<ID3D12GraphicsCommandList> commandList;
  ComPtr<IDXGISwapChain3> swapChain;
  ComPtr<ID3D12DescriptorHeap> rtvHeap;
  ComPtr<ID3D12Resource> m_swapChainBuffer[swapChainBufferCount];
  ComPtr<ID3D12DescriptorHeap> dsvHeap;
  ComPtr<ID3D12Resource> depthStencilBuffer;
  D3D12_VIEWPORT viewport = {};
  D3D12_RECT scissorRect = {};

  UINT64 currentFence = 0;
  HANDLE fenceEvent = nullptr;

  UINT rtvDescriptorSize = 0;
  UINT dsvDescriptorSize = 0;
  UINT cbvSrvUavDescriptorSize = 0;
public:
  bool Initialize(HWND hWnd, UINT width, UINT height);

  void Clear(const float col[4]);
  void PrepareRt();
  void Display();
  void Update(float dt);

};

#endif // D12_DRAW_GRAPHICSDEVICE_H
