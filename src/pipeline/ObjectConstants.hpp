#ifndef D12_DRAW_OBJECTCONSTANTS_H
#define D12_DRAW_OBJECTCONSTANTS_H

#include <DirectXMath.h>

struct ObjectConstants
{
  DirectX::XMFLOAT4X4 world;
  DirectX::XMFLOAT4X4 worldViewProj;

  DirectX::XMFLOAT3 eyePosW;
  float pad0;

  DirectX::XMFLOAT3 lightDirection;
  float pad1;
  DirectX::XMFLOAT4 lightColor;

  DirectX::XMFLOAT4 ambientColor;
  DirectX::XMFLOAT4 diffuseColor;
  DirectX::XMFLOAT4 specularColor;
  float shininess;
  DirectX::XMFLOAT3 pad2;
};

#endif // D12_DRAW_OBJECTCONSTANTS_H
