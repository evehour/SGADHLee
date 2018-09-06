#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};

PixelInput VS(VertexTexture input)
{
    PixelInput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    //NDC °ø°£ - FVF_RHWXYZ
    //output.Position = input.Position;
    output.Uv = input.Uv;

    return output;
}

///////////////////////////////////////////////////////////////////////////////

cbuffer PS_Select : register(b10)
{
    float padding;

    int Width;
    int Height;
    float MosaicLevel;
}

SamplerState Sampler : register(s10);
Texture2D Map : register(t10);

static const int BlurCount = 20;
float4 PS(PixelInput input) : SV_TARGET
{
    float2 divValue, val;
    divValue = saturate(MosaicLevel);

    val = MosaicLevel > 0 ? ((float2) ((int2) (input.Uv / divValue)) * divValue) : input.Uv;
    float4 color = Map.Sample(Sampler, val);

    return color;
}

float4 PS2(PixelInput input) : SV_TARGET
{
    float2 dDivValue, divValue;
    dDivValue = float2((float) Width * 10.0f / 100.0f, (float) Height * 10.0f / 100.0f);
    divValue = float2(1.0f / dDivValue.x, 1.0f / dDivValue.y);
    
    float2 val = (float2) ((int2) (input.Uv / divValue) * divValue);
    
    float4 color = Map.Sample(Sampler, val);

    return color;
}
