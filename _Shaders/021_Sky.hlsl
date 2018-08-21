#include "000_Header.hlsl"

cbuffer PS_Sky : register(b10)
{
	float4 Center;
	float4 Apex;

	float Height;
}

struct PixelInput
{
	float4 Position : SV_POSITION;
	float4 oPosition : POSITION0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;
    matrix world = Bones[BoneIndex];
    output.Position = mul(input.Position, world);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.oPosition = input.Position;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
	float y = saturate(input.oPosition.y);

    return lerp(Center, Apex, y * Height);
}