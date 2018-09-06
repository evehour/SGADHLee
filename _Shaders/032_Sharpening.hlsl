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
static const float3x3 SharpeningLevel[4] =
{
    {
        1, -2, 1,
        -2, 5, -2,
        1, -2, 1,
    },
    {
        0, -1, 0,
        -1, 5, -1,
        0, -1, 0,
    },
    {
        -1, -1, -1,
        -1, 9, -1,
        -1, -1, -1,
    },
    {
        1, 1, 1,
        1, -7, 1,
        1, 1, 1,
    }
};

float4 PS(PixelInput input) : SV_TARGET
{
    float3x3 MaskGroup = SharpeningLevel[Level];

    float2 ratio = float2(1.0f / (float) Width, 1.0f / (float) Height);
    float3 monotone = float3(0.299f, 0.587f, 0.114f);
    
    float4 color = 0; // = Map.Sample(Sampler, input.Uv);
    
    float x = input.Uv.x;
    float y = input.Uv.y;
        
    float2 temp = float2(x - ratio.x, y - ratio.y);
    float4 dcolor = Map.Sample(Sampler, temp);
    color += dcolor * MaskGroup[0][0];
        
    temp = float2(x, y - ratio.y);
    dcolor = Map.Sample(Sampler, temp);
    color += dcolor * MaskGroup[0][1];
        
    temp = float2(x + ratio.x, y - ratio.y);
    dcolor = Map.Sample(Sampler, temp);
    color += dcolor * MaskGroup[0][2];
        
    temp = float2(x - ratio.x, y);
    dcolor = Map.Sample(Sampler, temp);
    color += dcolor * MaskGroup[1][0];
        
    temp = float2(x, y);
    dcolor = Map.Sample(Sampler, temp);
    color += dcolor * MaskGroup[1][1];
        
    temp = float2(x + ratio.x, y);
    dcolor = Map.Sample(Sampler, temp);
    color += dcolor * MaskGroup[1][2];
        
    temp = float2(x - ratio.x, y + ratio.y);
    dcolor = Map.Sample(Sampler, temp);
    color += dcolor * MaskGroup[2][0];
        
    temp = float2(x, y + ratio.y);
    dcolor = Map.Sample(Sampler, temp);
    color += dcolor * MaskGroup[2][1];
        
    temp = float2(x + ratio.x, y + ratio.y);
    dcolor = Map.Sample(Sampler, temp);
    color += dcolor * MaskGroup[2][2];

    color = saturate(color);
    color.a = 1.0f;

    return color;
}
