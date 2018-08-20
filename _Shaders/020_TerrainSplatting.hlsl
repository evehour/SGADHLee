#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);

    output.Uv = input.Uv;

    return output;
}

Texture2D ColorMap : register(t10);
SamplerState ColorSampler : register(s10);

Texture2D ColorMap2 : register(t11);
SamplerState ColorSampler2 : register(s11);

Texture2D AlphaMap : register(t12);
SamplerState AlphaSampler : register(s12);

float4 PS(PixelInput input) : SV_TARGET
{   
    float4 color = 0;

	float4 colorMap = ColorMap.Sample(ColorSampler, input.Uv);
	float4 colorMap2 = ColorMap2.Sample(ColorSampler2, input.Uv);
	float4 alphaMap = AlphaMap.Sample(AlphaSampler, input.Uv);

	float4 alpha = float4(alphaMap.r, alphaMap.r, alphaMap.r, alphaMap.r);
	float4 temp = lerp(colorMap, colorMap2, alpha);

    DiffuseLighting(color, temp, input.Normal);

    return color;
}