cbuffer cbPerObject : register(b0)
{
  float4x4 gWorld;
  float4x4 gWorldViewProj;

  float3 gEyePosW;
  float _pad0;

  float3 gLightDirection;
  float _pad1;
  float4 gLightColor;

  float4 gAmbientColor;
  float4 gDiffuseColor;
  float4 gSpecularColor;
  float gShininess;
};

struct VertexIn
{
  float3 PosL : POSITION;
  float3 NormalL : NORMAL;
  float3 Color : COLOR;
};

struct VertexOut
{
  float4 PosH : SV_POSITION;
  float3 PosW : POSITION;
  float3 NormalW : NORMAL;
  float3 Color : COLOR;
};

VertexOut vert(VertexIn vin)
{
  VertexOut vout;

  float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

  vout.PosW = posW.xyz;
  vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
  vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
  vout.Color = vin.Color;

  return vout;
}

float4 frag(VertexOut pin) : SV_TARGET
{
  float3 N = normalize(pin.NormalW);
  float3 L = normalize(-gLightDirection);
  float3 V = normalize(gEyePosW - pin.PosW);
  float3 R = reflect(-L, N);

  float4 ambient = gAmbientColor * gLightColor;

  float diffuseFactor = max(dot(N, L), 0.0f);
  float4 diffuse = diffuseFactor * float4(pin.Color, 1.0f) * gDiffuseColor * gLightColor;

  float specularFactor = pow(max(dot(R, V), 0.0f), gShininess);
  float4 specular = specularFactor * gSpecularColor * gLightColor;

  specular *= (diffuseFactor > 0.0f) ? 1.0f : 0.0f;

  float4 result = ambient + diffuse + specular;
  result.a = 1.0f;

  return result;
}