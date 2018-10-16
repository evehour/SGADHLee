#include "000_Header.hlsl"

cbuffer VS_Project : register(b10)
{
    matrix V;
    matrix P;
}

struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
    float4 pPosition : POSITION0; // 프로젝션된 포지션
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    matrix world = BoneWorld();
    output.Position = mul(input.Position, world);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = WorldNormal(input.Normal, world);

    output.Uv = input.Uv;

    output.pPosition = mul(input.Position, world);
    output.pPosition = mul(output.pPosition, V);
    output.pPosition = mul(output.pPosition, P);
    // 레스터라이징 단계에서 뷰포트, 프로젝션 변환을 거치고 나면
    // 원본 비율이 있는데 그 원본비율이 w임.

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    DiffuseLighting(color, diffuse, input.Normal);

    float2 projUv = 0;
    projUv.x = input.pPosition.x / input.pPosition.w * 0.5f + 0.5f;
    projUv.y = -input.pPosition.y / input.pPosition.w * 0.5f + 0.5f;

	// x^2 + y^2 = r^2 원의 방정식
    color = ((projUv.x - 0.5f) * (projUv.x - 0.5f) + (projUv.y - 0.5f) * (projUv.y - 0.5f) <= 1.0f) ? NormalMap.Sample(NormalSampler, projUv) : color;
    //color = (saturate(projUv.x) == projUv.x && saturate(projUv.y) == projUv.y) ? NormalMap.Sample(NormalSampler, projUv) : color;
    
    return color;
}