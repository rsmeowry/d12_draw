
#include "GraphicsDevice.h"
#include <comdef.h>
#include "Util.h"

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
