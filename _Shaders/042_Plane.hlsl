#include "000_Header.hlsl"

cbuffer PS_Select : register(b10)
{
    int Selected;
}

struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    matrix world = BoneWorld();
    output.Position = mul(input.Position, world);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;

    output.Normal = WorldNormal(input.Normal, world);

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    //float4 color = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    float4 color = 0;

    DiffuseLighting(color, input.Normal);


    return color;
}