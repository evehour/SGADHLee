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
    int Width;
    int Height;
    float2 padding;

    float3 Rgb;
    float padding2;
}

SamplerState Sampler : register(s10);
Texture2D Map : register(t10);

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = Map.Sample(Sampler, input.Uv);
    float3 dRgb = Rgb;
    color.r = pow(color.r, dRgb.r);
    color.g = pow(color.g, dRgb.g);
    color.b = pow(color.b, dRgb.b);

    return color;
}
