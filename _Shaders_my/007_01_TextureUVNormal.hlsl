cbuffer VS_ViewProjection : register(b0)
{
    matrix _view;
    matrix _projection;
}

cbuffer VS_World : register(b1)
{
    matrix _world;
}

cbuffer PS_COLOR : register(b0)
{
    float4 Color;
}

cbuffer PS_HALFUV : register(b1)
{
    float2 halfUV;
}

struct VertexInput
{
    float4 position : POSITION0;
    float2 uv : UV0;
    float3 normal : NORMAL;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : UV0;
    float3 normal : NORMAL;
};

SamplerState Sampler : register(s0);
Texture2D Map : register(t0);
Texture2D Map2 : register(t1);

PixelInput VS(VertexInput input)
{
    PixelInput output;
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.uv = input.uv;

    output.normal = mul(input.normal, (float3x3) _world);
    output.normal = normalize(output.normal);

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = Map.Sample(Sampler, input.uv);
    float4 color2 = Map2.Sample(Sampler, input.uv);

    float x = input.uv.x;

    //color = (x < halfUV.x) ? color2 : color;
    //color = Color;

    return color;
}
