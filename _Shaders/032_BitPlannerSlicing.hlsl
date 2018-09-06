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
    int Level;
    int Width;
    int Height;

    float padding;
}

SamplerState Sampler : register(s10);
Texture2D Map : register(t10);

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = Map.Sample(Sampler, input.Uv);
    uint4 dcolor = color * 255;
    
    dcolor >>= Level;
    dcolor <<= Level;

    color = (float4) dcolor / 255.0f;

    return color;
}
