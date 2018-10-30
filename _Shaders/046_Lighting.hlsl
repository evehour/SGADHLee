#include "000_Header2.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
    float4 wPosition : POSITION0;
    float3 cPosition : POSITION1;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    matrix world = BoneWorld();
    output.Position = mul(input.Position, world);
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;

    output.Normal = WorldNormal(input.Normal, world);
    output.cPosition = CameraPosition();

    return output;
}

//Ambient
float4 PS(PixelInput input) : SV_TARGET
{
    float3 normal = normalize(input.Normal);
    float4 color = float4(Diffuse.rgb * Diffuse.rgb, Diffuse.a); // 여기서의 alpha는 투명이 아니라 강도.

    Material material = CreateMaterial(input.Normal, input.Uv);
    color.rgb = Lighting(input.wPosition.xyz, input.cPosition, material);
    color.a = 1.0f;

    return color;
}
