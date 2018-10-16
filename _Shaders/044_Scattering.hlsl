#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float4 oPosition : POSITION0;
};

PixelInput VS_Scattering(VertexTexture input)
{
    PixelInput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.oPosition = input.Position;

    return output;
}

float4 PS_Scattering(PixelInput input) : SV_TARGET
{
    return float4(1, 1, 1, 1);
}

///////////////////////////////////////////////////////////////////////////////

struct PixelTargetInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};

PixelTargetInput VS_Target(VertexTexture input)
{
    PixelTargetInput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;

    return output;
}

float4 PS_Target(PixelTargetInput input) : SV_TARGET
{
    return float4(1, 1, 1, 1);
}
