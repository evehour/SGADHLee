#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
	float4 Color : COLOR0;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
};

PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;

    matrix world = World; //Bones[BoneIndex];
    output.Position = mul(input.Position, world);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) world);
    output.Normal = normalize(output.Normal);
	
	output.Color = input.Color;

    output.Uv = input.Uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;

    float4 diffuse = input.Color; //DiffuseMap.Sample(DiffuseSampler, input.Uv);
    DiffuseLighting(color, diffuse, input.Normal);

    return color;
}