#include "000_Header.hlsl"


struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
};

Texture2D Map2 : register(t1);
Texture2D Map3 : register(t2);

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    // 노멀은 위치가 아니고 방향이므로 4x4를 쓸 필요가 없다)
    output.Normal = mul(input.Normal, (float3x3) World);

    output.Uv = input.Uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{

    float4 t = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    float4 t2 = Map2.Sample(DiffuseSampler, input.Uv);
    float4 alpha = Map3.Sample(DiffuseSampler, input.Uv);

    float4 diffuse = (1 - alpha.r) * t + t2 * alpha.r;
    float4 color = 0;
    DiffuseLight(color, diffuse, input.Normal);
    //DiffuseLight(color, input.Normal);
    
    return color;
    //return float4(1, 1, 1, 1);
    //return float4((input.Normal * 0.5f + 0.5f), 1);
}
