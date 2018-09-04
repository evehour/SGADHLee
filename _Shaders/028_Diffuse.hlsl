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

    matrix world = Bones[BoneIndex];
    output.Position = mul(input.Position, world);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) world);

    output.Uv = input.Uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    
    [branch]
    if(length(diffuse) < 0.01f)
        DiffuseLighting(color, input.Normal);
    else
        DiffuseLighting(color, diffuse, input.Normal);

    return color;
}