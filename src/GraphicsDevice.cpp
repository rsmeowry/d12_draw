
#include "GraphicsDevice.h"
#include <comdef.h>

#include "D3DApp.h"
#include "Util.h"

bool GraphicsDevice::Initialize(HWND hWnd, UINT width, UINT height) {
  if (!InitDevice())
    return false;

  CreateFence();
  CacheDescriptorSizes();
  CreateCommandObjects();
  CreateSwapChain(hWnd, width, height);
  CreateRtvHeap();
  CreateRenderViews();

  return true;
}

bool GraphicsDevice::InitDevice() {
#if defined(DEBUG) || defined(_DEBUG)
  {
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
      debugController->EnableDebugLayer();
    }
  }
#endif

  ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));

  HRESULT hr = D3D12CreateDevice(
    nullptr,
    D3D_FEATURE_LEVEL_11_0,
    IID_PPV_ARGS(&d3dDevice)
    );

  // warp fallback
  if (FAILED(hr)) {
    ComPtr<IDXGIAdapter> warpAdapter;
    ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

    ThrowIfFailed(D3D12CreateDevice(
      warpAdapter.Get(),
      D3D_FEATURE_LEVEL_12_0,
      IID_PPV_ARGS(&d3dDevice))
      );
  }

  return true;
}

void GraphicsDevice::CreateFence() {
  ThrowIfFailed(d3dDevice->CreateFence(
    0,
    D3D12_FENCE_FLAG_NONE,
    IID_PPV_ARGS(&fence))
    );

  fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void GraphicsDevice::CacheDescriptorSizes() {
  rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  dsvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
  cbvSrvUavDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void GraphicsDevice::CreateCommandObjects() {
  D3D12_COMMAND_QUEUE_DESC queueDesc = {};
  queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
  queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

  ThrowIfFailed(d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
  ThrowIfFailed(d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAlloc)));
  ThrowIfFailed(d3dDevice->CreateCommandList(
    0,
    D3D12_COMMAND_LIST_TYPE_DIRECT,
    commandAlloc.Get(),
    nullptr,
    IID_PPV_ARGS(&commandList))
    );

  ThrowIfFailed(commandList->Close());
}

void GraphicsDevice::CreateSwapChain(HWND hWnd, UINT width, UINT height) {
  swapChain.Reset();

  DXGI_SWAP_CHAIN_DESC1 sd = {};
  sd.Width = width;
  sd.Height = height;
  sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.BufferCount = swapChainBufferCount;
  sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  sd.Flags = 0;

  ComPtr<IDXGISwapChain1> swapChain1;
  ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
    commandQueue.Get(),
    hWnd,
    &sd,
    nullptr,
    nullptr,
    &swapChain1));

  ThrowIfFailed(swapChain1.As(&d3dDevice));

  currBackBuffer = swapChain->GetCurrentBackBufferIndex();
}

void GraphicsDevice::CreateRtvHeap() {
  D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
  rtvHeapDesc.NumDescriptors = swapChainBufferCount;
  rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

  ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));
}

void GraphicsDevice::CreateRenderViews() {
  D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
    rtvHeap->GetCPUDescriptorHandleForHeapStart();

  for (UINT i = 0; i < swapChainBufferCount; i++) {
    ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainBuffer[i])));

    d3dDevice->CreateRenderTargetView(
      m_swapChainBuffer[i].Get(),
      nullptr,
      rtvHandle
      );

    rtvHandle.ptr += rtvDescriptorSize;
  }
}


void GraphicsDevice::FlushCommandQueue() {
  currentFence++;

  ThrowIfFailed(commandQueue->Signal(fence.Get(), currentFence));

  if (fence->GetCompletedValue() < currentFence) {
    ThrowIfFailed(fence->SetEventOnCompletion(currentFence, fenceEvent));
    WaitForSingleObject(fenceEvent, INFINITE);
  }
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDevice::GetCurrentBbView() const {
  D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
  handle.ptr += currBackBuffer * rtvDescriptorSize;
  return handle;
}

ID3D12Resource* GraphicsDevice::GetCurrentBb() const {
  return m_swapChainBuffer[currBackBuffer].Get();
}