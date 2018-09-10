#include "000_Header.hlsl"

cbuffer PS_NORMAL : register(b10)
{
    int VisibleNormal;
}

struct PixelInput
{
    float4 Position : SV_POSITION;
    float4 wPosition : POSITION0;
    float4 pPosition : POSITION1;
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

    output.pPosition = output.Position;

    output.Uv = input.Uv;

    output.Normal = WorldNormal(input.Normal, world);
    output.Tangent = WorldTangent(input.Tangent, world);

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    float detail = input.Position.z / input.pPosition.w;
    
    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    DiffuseLighting(color, diffuse, input.Normal);
    
    [branch]
    if(detail > 0.1f)
    {
        float4 detailMap = DetailMap.Sample(DetailSampler, input.Uv);
        DetailMapping(color, detailMap, 1.7f);
        
        float4 normal = NormalMap.Sample(NormalSampler, input.Uv);
        NormalMapping(color, normal, input.Normal, input.Tangent);
    }
    else if(detail > 0.05f)
    {
        float4 normal = NormalMap.Sample(NormalSampler, input.Uv);
        NormalMapping(color, normal, input.Normal, input.Tangent);
    }

    SpecularLighting(color, input.Normal, input.ViewDir);
   
    //float4 specular = SpecularMap.Sample(SpecularSampler, input.Uv);
    //SpecularLighting(color, specular, input.Normal, input.ViewDir);

    for (int i = 0; i < PointLightCount; i++)
        PointLighting(color, PointLights[i], input.wPosition, input.Normal);

    color.a = 1.0f;

    return color;
}