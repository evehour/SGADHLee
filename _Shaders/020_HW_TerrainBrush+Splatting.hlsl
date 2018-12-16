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
	float4 Color : Color1;
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

	if (Type >= 2)
	{
		float dx = location.x - Location.x;
		float dy = location.z - Location.z;

		float dist = sqrt(dx * dx + dy * dy);

		if (dist <= Range)
			return Color;
	}

	return float3(0, 0, 0);
}

PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);

	output.BrushColor = BrushColor(input.Position.xyz);

    output.Uv = input.Uv;
	output.Color = input.Color;

    return output;
}

Texture2D AlphaMap : register(t10);
SamplerState AlphaSampler : register(s10);

Texture2D ColorMap : register(t11);
SamplerState ColorSampler : register(s11);

Texture2D ColorMap2 : register(t12);
SamplerState ColorSampler2 : register(s12);

Texture2D ColorMap3 : register(t13);
SamplerState ColorSampler3 : register(s13);

float4 PS(PixelInput input) : SV_TARGET
{   
    float4 color = 0;

	float4 colorMap = ColorMap.Sample(ColorSampler, input.Uv);
	float4 colorMap2 = ColorMap2.Sample(ColorSampler2, input.Uv);
	float4 colorMap3 = ColorMap3.Sample(ColorSampler3, input.Uv);
    float4 alphaMap = input.Color; //AlphaMap.Sample(AlphaSampler, input.Uv);

	float4 alpha = float4(alphaMap.r, alphaMap.r, alphaMap.r, alphaMap.r);
	float4 temp = lerp(colorMap, colorMap2, alpha);

	alpha = float4(alphaMap.g, alphaMap.g, alphaMap.g, alphaMap.g);
	temp = lerp(temp, colorMap3, alpha);

    DiffuseLighting(color, temp, input.Normal);

    color = colorMap;
	color = color + float4(input.BrushColor, 0);

    return color;
}