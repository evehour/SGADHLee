#include "000_Header.hlsl"

cbuffer VS_Brush : register(b10)
{
	int Type;
	float3 Location;

	int Range;
	float3 Color;
}

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;

	float3 BrushColor : COLOR0;
};

float3 BrushColor(float3 location)
{
	if (Type == 0)
		return float3(0, 0, 0);

	if (Type == 1)
	{
		if ((location.x >= (Location.x - Range)) &&
			(location.x <= (Location.x + Range)) &&
			(location.z >= (Location.z - Range)) &&
			(location.z <= (Location.z + Range)))
		{
			return Color;
		}
	}

	if (Type == 2)
	{
		float dx = location.x - Location.x;
		float dy = location.z - Location.z;

		float dist = sqrt(dx * dx + dy * dy);

		if (dist <= Range)
			return Color;
	}

	return float3(0, 0, 0);
}

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);

	output.BrushColor = BrushColor(input.Position.xyz);

    output.Uv = input.Uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{   
    float4 color = 0;

    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    DiffuseLighting(color, diffuse, input.Normal);

	color = color + float4(input.BrushColor, 0);

    return color;
}