#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};

SamplerState Sampler : register(s10);
Texture2D Map : register(t10);

PixelInput VS(VertexTexture input)
{
    PixelInput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    //NDC 공간 - FVF_RHWXYZ
	//output.pPosition.xyz = output.Position.xyz / output.Position.w * 0.5f + 0.5f;
	//이후에 PS에서 UV값을 pPosition.x, pPosition.y를 사용하여 씀.
    output.Uv = input.Uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = Map.Sample(Sampler, input.Uv);

    return color;
}