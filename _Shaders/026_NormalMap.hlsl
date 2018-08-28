#include "000_Header.hlsl"

cbuffer PS_NORMAL : register(b10)
{
    int VisibleNormal;
}

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
    float3 ViewDir : VIEWDIR0;
};

PixelInput VS(VertexTextureNormalTangent input)
{
    PixelInput output;

    matrix world = Bones[BoneIndex];
    output.Position = mul(input.Position, world);

    output.ViewDir = WorldViewDirection(output.Position);

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;

    output.Normal = WorldNormal(input.Normal, world);
    output.Tangent = WorldTangent(input.Tangent, world);

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    
    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    DiffuseLighting(color, diffuse, input.Normal);

    if (VisibleNormal == 1)
    {
        float4 normal = NormalMap.Sample(NormalSampler, input.Uv);
        NormalMapping(color, normal, input.Normal, input.Tangent);
    }
   
    float4 specular = SpecularMap.Sample(SpecularSampler, input.Uv);
    SpecularLighting(color, specular, input.Normal, input.ViewDir);

    color.a = 1.0f;

    return color;
}