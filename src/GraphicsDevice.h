#ifndef D12_DRAW_GRAPHICSDEVICE_H
#define D12_DRAW_GRAPHICSDEVICE_H

#define INITGUID
#include <SimpleMath.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#include "pipeline/ObjectConstants.hpp"
#include "pipeline/UploadBuffer.h"

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
  void CreateCBuffer();
  void CreateRootSignature();
  void FlushCommandQueue();
  void CompileShaders();
  void CreatePSO();

  D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBbView() const;
  ID3D12Resource *GetCurrentBb() const;
  D3D12_CPU_DESCRIPTOR_HANDLE GetDsv() const;

  static constexpr auto shaderName = L"shaders/Phong.hlsl";
  static constexpr DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
  static constexpr DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
  static constexpr UINT swapChainBufferCount = 2;
  UINT currBackBuffer = 0;

  // base shit
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

  // vb/ib
  ComPtr<ID3D12Resource> vertexBufferGPU;
  ComPtr<ID3D12Resource> vertexBufferUploader;
  ComPtr<ID3D12Resource> indexBufferGPU;
  ComPtr<ID3D12Resource> indexBufferUploader;

  // shaders
  ComPtr<ID3DBlob> vsByteCode;
  ComPtr<ID3DBlob> fsByteCode;

  // CB
  std::unique_ptr<UploadBuffer<ObjectConstants>> objectCB;
  ComPtr<ID3D12DescriptorHeap> cbvHeap;
  ComPtr<ID3D12RootSignature> rootSignature;

  // pipelinee
  ComPtr<ID3D12PipelineState> pso;

  D3D12_VERTEX_BUFFER_VIEW vbv = {};
  D3D12_INDEX_BUFFER_VIEW ibv = {};

  UINT indexCount = 0;
  UINT64 currentFence = 0;
  HANDLE fenceEvent = nullptr;

  UINT rtvDescriptorSize = 0;
  UINT dsvDescriptorSize = 0;
  UINT cbvSrvUavDescriptorSize = 0;

  void CubeGeom1();
  void CubeGeom2();

public:
  bool Initialize(HWND hWnd, UINT width, UINT height);

  void Clear(const float col[4]);
  void PrepareRt();
  void Display();
  void Update(float dt) const;
  void RenderContainedObject() const;
};

#endif // D12_DRAW_GRAPHICSDEVICE_H
