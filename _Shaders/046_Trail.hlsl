#include "000_Header.hlsl"

cbuffer VS_TrailMatrix : register(b10)
{
    matrix TrailMatrix[128];

    int TrailCount;
    float3 VS_TrailMatrix_Padding;
}

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};

PixelInput VS(VertexTexture input)
{
    PixelInput output;

    float rate = 1.0f / (float) TrailCount;
    int idx = (int) ((input.Uv.x) / rate);
    idx = min(idx, TrailCount - 1);

    matrix mat = TrailMatrix[idx];

    output.Position = mul(input.Position, mat);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;

    return output;
}

SamplerState Sampler : register(s10);
Texture2D trailTex : register(t10);

float4 PS(PixelInput input) : SV_TARGET
{
    input.Uv.y = 1.0f - input.Uv.y;
    float4 color = trailTex.Sample(Sampler, input.Uv);

    return color;
}