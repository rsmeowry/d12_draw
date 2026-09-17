
#include "GraphicsDevice.h"

#include <array>
#include <comdef.h>

#include "Util.h"
#include "pipeline/Vertex.hpp"

void GraphicsDevice::TestCubeGeom() {
  std::array<Vertex1, 8> vertices = {
      Vertex1{{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
      Vertex1{{-1.0f, +1.0f, -1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
      Vertex1{{+1.0f, +1.0f, -1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
      Vertex1{{+1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}},
      Vertex1{{-1.0f, -1.0f, +1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}},
      Vertex1{{-1.0f, +1.0f, +1.0f}, {0.0f, 1.0f, 1.0f, 1.0f}},
      Vertex1{{+1.0f, +1.0f, +1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
      Vertex1{{+1.0f, -1.0f, +1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
  };

  std::array<std::uint16_t, 36> indices = {
      0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6, 4, 5, 1, 4, 1, 0, 3, 2, 6, 3, 6, 7, 1, 5, 6, 1, 6, 2, 4, 0, 3, 4, 3, 7,
  };

  const UINT vbByteSize = vertices.size() * sizeof(Vertex1);
  const UINT ibByteSize = indices.size() * sizeof(std::uint16_t);

  vertexBufferGPU =
      CreateDefaultBuffer(d3dDevice.Get(), commandList.Get(), vertices.data(), vbByteSize, vertexBufferUploader);

  indexBufferGPU =
      CreateDefaultBuffer(d3dDevice.Get(), commandList.Get(), indices.data(), ibByteSize, indexBufferUploader);

  vbv.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
  vbv.StrideInBytes = sizeof(Vertex1);
  vbv.SizeInBytes = vbByteSize;

  ibv.BufferLocation = indexBufferGPU->GetGPUVirtualAddress();
  ibv.Format = DXGI_FORMAT_R16_UINT;
  ibv.SizeInBytes = ibByteSize;

  indexCount = static_cast<UINT>(indices.size());
}

bool GraphicsDevice::Initialize(HWND hWnd, UINT width, UINT height) {
  if (!InitDevice())
    return false;

  OutputDebugStringA("CREATE FENCE");
  CreateFence();
  OutputDebugStringA("CACHE");
  CacheDescriptorSizes();
  OutputDebugStringA("CO");
  CreateCommandObjects();
  OutputDebugStringA("SC");
  CreateSwapChain(hWnd, width, height);
  OutputDebugStringA("RTV HEAP");
  CreateRtvHeap();
  OutputDebugStringA("RTV");
  CreateRenderViews();
  CreateDsvHeap();

  // opening alloc to set barrier from dsv
  ThrowIfFailed(commandList->Reset(commandAlloc.Get(), nullptr));
  CreateDepthStencilBuffer(width, height);
  TestCubeGeom();
  commandList->Close(); // and closing

  ID3D12CommandList *cmdLists[] = {commandList.Get()};
  commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
  FlushCommandQueue(); // saving barrier

  SetViewportAndScissor(width, height);

  // shaders !!!!!!11
  CompileShaders();

  // cbuffer
  CreateCBuffer();
  CreateRootSignature();
  CreatePSO();

  return true;
}
void GraphicsDevice::Clear(const float col[4]) {
  D3D12_CPU_DESCRIPTOR_HANDLE rtv = GetCurrentBbView();
  D3D12_CPU_DESCRIPTOR_HANDLE dsv = GetDsv();

  commandList->OMSetRenderTargets(1, &rtv, TRUE, &dsv);
  commandList->ClearRenderTargetView(rtv, col, 0, nullptr);
  commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void GraphicsDevice::PrepareRt() {
  ThrowIfFailed(commandAlloc->Reset());
  ThrowIfFailed(commandList->Reset(commandAlloc.Get(), pso.Get()));

  // present -> rt
  CD3DX12_RESOURCE_BARRIER toRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(
      GetCurrentBb(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
  commandList->ResourceBarrier(1, &toRenderTarget);

  commandList->RSSetViewports(1, &viewport);
  commandList->RSSetScissorRects(1, &scissorRect);
}

void GraphicsDevice::Display() {
  // rt -> present
  CD3DX12_RESOURCE_BARRIER toPresent = CD3DX12_RESOURCE_BARRIER::Transition(
      GetCurrentBb(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
  commandList->ResourceBarrier(1, &toPresent);

  ThrowIfFailed(commandList->Close());

  ID3D12CommandList *cmdLists[] = {commandList.Get()};
  commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

  ThrowIfFailed(swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING));
  currBackBuffer = swapChain->GetCurrentBackBufferIndex();

  FlushCommandQueue();
}

void GraphicsDevice::Update(const float dt) const {
  using namespace DirectX;

  static float angle = 0.0f;
  angle += dt;

  XMMATRIX world = XMMatrixRotationY(angle);
  XMMATRIX view = XMMatrixLookAtLH(XMVectorSet(3.0f, 3.0f, -5.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
                                   XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
  XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, viewport.Width / viewport.Height, 1.0f, 100.0f);

  XMMATRIX worldViewProj = world * view * proj;

  ObjectConstants objConstants;
  XMStoreFloat4x4(&objConstants.worldViewProj, XMMatrixTranspose(worldViewProj));
  objectCB->CopyData(0, objConstants);
}

void GraphicsDevice::RenderContainedObject() const {
  commandList->SetGraphicsRootSignature(rootSignature.Get());

  ID3D12DescriptorHeap *heaps[] = {cbvHeap.Get()};
  commandList->SetDescriptorHeaps(_countof(heaps), heaps);

  commandList->SetGraphicsRootDescriptorTable(0, cbvHeap->GetGPUDescriptorHandleForHeapStart());

  commandList->IASetVertexBuffers(0, 1, &vbv);
  commandList->IASetIndexBuffer(&ibv);
  commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
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

  ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory)));

  HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice));

  // warp fallback
  if (FAILED(hr)) {
    ComPtr<IDXGIAdapter> warpAdapter;
    ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

    ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&d3dDevice)));
  }

  return true;
}

void GraphicsDevice::CreateFence() {
  ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

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
  ThrowIfFailed(d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAlloc.Get(), nullptr,
                                             IID_PPV_ARGS(&commandList)));

  ThrowIfFailed(commandList->Close());
}

void GraphicsDevice::CreateSwapChain(HWND hWnd, UINT width, UINT height) {
  swapChain.Reset();

  DXGI_SWAP_CHAIN_DESC1 sd = {};
  sd.Width = width;
  sd.Height = height;
  sd.Format = backBufferFormat;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.BufferCount = swapChainBufferCount;
  sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

  ComPtr<IDXGISwapChain1> swapChain1;
  ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), hWnd, &sd, nullptr, nullptr, &swapChain1));

  ThrowIfFailed(swapChain1.As(&swapChain));

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
  D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();

  for (UINT i = 0; i < swapChainBufferCount; i++) {
    ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainBuffer[i])));

    d3dDevice->CreateRenderTargetView(m_swapChainBuffer[i].Get(), nullptr, rtvHandle);

    rtvHandle.ptr += rtvDescriptorSize;
  }
}

void GraphicsDevice::CreateDsvHeap() {
  D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
  dsvHeapDesc.NumDescriptors = 1;
  dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
  dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

  ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap)));
}

void GraphicsDevice::CreateDepthStencilBuffer(UINT width, UINT height) {
  D3D12_RESOURCE_DESC depthStencilDesc = {};
  depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
  depthStencilDesc.Alignment = 0;
  depthStencilDesc.Width = width;
  depthStencilDesc.Height = height;
  depthStencilDesc.DepthOrArraySize = 1;
  depthStencilDesc.MipLevels = 1;
  depthStencilDesc.Format = depthStencilFormat;
  depthStencilDesc.SampleDesc.Count = 1;
  depthStencilDesc.SampleDesc.Quality = 0;
  depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
  depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

  D3D12_CLEAR_VALUE optClear = {};
  optClear.Format = depthStencilFormat;
  optClear.DepthStencil.Depth = 1.0f;
  optClear.DepthStencil.Stencil = 0;

  CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

  ThrowIfFailed(d3dDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &depthStencilDesc,
                                                   D3D12_RESOURCE_STATE_COMMON, &optClear,
                                                   IID_PPV_ARGS(&depthStencilBuffer)));

  OutputDebugStringA("BEFORE");
  d3dDevice->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, dsvHeap->GetCPUDescriptorHandleForHeapStart());
  OutputDebugStringA("AFTER");

  CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
      depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);

  commandList->ResourceBarrier(1, &barrier);
}

void GraphicsDevice::SetViewportAndScissor(UINT width, UINT height) {
  viewport.TopLeftX = 0.;
  viewport.TopLeftY = 0.;
  viewport.Width = static_cast<float>(width);
  viewport.Height = static_cast<float>(height);
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f; // linear depth

  scissorRect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
}

void GraphicsDevice::CreateCBuffer() {
  D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
  cbvHeapDesc.NumDescriptors = 1;
  cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
  cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

  ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&cbvHeap)));

  objectCB = std::make_unique<UploadBuffer<ObjectConstants>>(d3dDevice.Get(), 1, true);

  D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
  cbvDesc.BufferLocation = objectCB->GetElementAddress(0);
  cbvDesc.SizeInBytes = objectCB->GetElementByteSize();
  d3dDevice->CreateConstantBufferView(&cbvDesc, cbvHeap->GetCPUDescriptorHandleForHeapStart());
}

void GraphicsDevice::CreateRootSignature() {
  CD3DX12_DESCRIPTOR_RANGE cbvTable;
  cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

  CD3DX12_ROOT_PARAMETER slotRootParameter[1];
  slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);


  CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr,
                                          D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

  ComPtr<ID3DBlob> serializedRootSig;
  ComPtr<ID3DBlob> errorBlob;

  HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSig, &errorBlob);

  if (errorBlob != nullptr) {
    OutputDebugStringA(static_cast<const char *>(errorBlob->GetBufferPointer()));
  }
  ThrowIfFailed(hr);

  ThrowIfFailed(d3dDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
                                               serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
}

void GraphicsDevice::FlushCommandQueue() {
  currentFence++;

  ThrowIfFailed(commandQueue->Signal(fence.Get(), currentFence));

  if (fence->GetCompletedValue() < currentFence) {
    ThrowIfFailed(fence->SetEventOnCompletion(currentFence, fenceEvent));
    WaitForSingleObject(fenceEvent, INFINITE);
  }
}

void GraphicsDevice::CompileShaders() {
  vsByteCode = CompileShader(L"shaders/Color.hlsl", nullptr, "vert", "vs_5_1");
  fsByteCode = CompileShader(L"shaders/Color.hlsl", nullptr, "frag", "ps_5_1");
}

void GraphicsDevice::CreatePSO() {
  D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

  psoDesc.InputLayout = {InputLayout1, _countof(InputLayout1)};
  psoDesc.pRootSignature = rootSignature.Get();

  psoDesc.VS = {
      reinterpret_cast<const BYTE *>(vsByteCode->GetBufferPointer()),
      vsByteCode->GetBufferSize(),
  };
  psoDesc.PS = {
      reinterpret_cast<const BYTE *>(fsByteCode->GetBufferPointer()),
      fsByteCode->GetBufferSize(),
  };

  psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
  psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
  psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
  psoDesc.SampleMask = UINT_MAX;
  psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

  psoDesc.NumRenderTargets = 1;
  psoDesc.RTVFormats[0] = backBufferFormat;
  psoDesc.DSVFormat = depthStencilFormat;

  psoDesc.SampleDesc.Count = 1;
  psoDesc.SampleDesc.Quality = 0;

  ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)));
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDevice::GetCurrentBbView() const {
  D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
  handle.ptr += currBackBuffer * rtvDescriptorSize;
  return handle;
}

ID3D12Resource *GraphicsDevice::GetCurrentBb() const { return m_swapChainBuffer[currBackBuffer].Get(); }

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDevice::GetDsv() const { return dsvHeap->GetCPUDescriptorHandleForHeapStart(); }
