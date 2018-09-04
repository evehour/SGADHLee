#include "000_Header.hlsl"

cbuffer PS_Line : register(b10)
{
    int LineType;
    float3 LineColor;

    int Spacing;
    float Thickness;
}

struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : POSITION0;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    output.oPosition = input.Position;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);
    output.Uv = input.Uv;

    return output;
}

float3 Line(float3 oPosition)
{
    [branch]
    if (LineType == 1)
    {
        float2 grid = 0;
        grid.x = frac(oPosition.x / (float) Spacing);
        grid.y = frac(oPosition.z / (float) Spacing);

        [flatten]
        if (grid.x < Thickness || grid.y < Thickness)
            return LineColor;
    }
    else if (LineType == 2)
    {
        float2 grid = oPosition.xz / (float) Spacing;

        float2 range = abs(frac(grid - 0.5f) - 0.5f);
        float2 speed = fwidth(grid);

        float2 pixel = range / speed;
        float weights = saturate(min(pixel.x, pixel.y) - Thickness);

        return lerp(LineColor, float3(0, 0, 0), weights);
    }

    return float3(0, 0, 0);
}

float4 PS(PixelInput input) : SV_TARGET
{   
    float4 color = 0;

    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    DiffuseLighting(color, diffuse, input.Normal);

    color = color + float4(Line(input.oPosition), 0);

    return color;
}