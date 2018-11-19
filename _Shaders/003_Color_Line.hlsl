#include "000_Header.hlsl"

cbuffer PS_Color : register(b10)
{
    float4 Color;
}

struct VertexInput
{
    float4 position : POSITION0;
    float4 color : COLOR0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

PixelInput VS(VertexInput input)
{
    PixelInput output;
    matrix world = World;
    output.position = mul(input.position, world);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);

    output.color = input.color;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    return input.color;
}