#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float4 wPosition : POSITION0;
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
    output.wPosition = output.Position;

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
    float4 colord = 0, colorn = 0, colors = 0, colorp = 0;
    
    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    DiffuseLighting(colord, diffuse, input.Normal);
    
    float4 normal = NormalMap.Sample(NormalSampler, input.Uv);
    NormalMapping(colorn, normal, input.Normal, input.Tangent);
    
    float4 specular = SpecularMap.Sample(SpecularSampler, input.Uv);
    SpecularLighting(colors, specular, input.Normal, input.ViewDir);

    int i = 0;
    for (i = 0; i < PointLightCount; i++)
        PointLighting(colorp, PointLights[i], input.wPosition, input.Normal);

    color = diffuse * (colord + colorn + colors + colorp);
    color.a = 1.0f;

    return color;
}