#include "000_Header2.hlsl"


struct VertexInput
{
    float4 Position : POSITION0;
    float2 Uv : UV0;

    float3 Instance : INSTANCE0;
    uint InstanceID : SV_INSTANCEID;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;

    uint id : INSTANCEID0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    
    output.Position.x = input.Position.x + input.InstanceID;
    output.Position.y = input.Position.y + 0;
    output.Position.z = input.Position.z + 0;
    output.Position.w = 1;

    output.Position = mul(output.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;
    output.id = input.InstanceID;

    return output;
}

Texture2D Map[6] : register(t10);
//Texture2DArray Map : register(t10);
SamplerState Sampler : register(s10);

const static int arrayIdx[6] = { 0, 1, 2, 3, 4, 5 };

float4 PS(VertexOutput input) : SV_TARGET
{
    //float4 color = Map[input.id].Sample(Sampler[input.id], input.Uv);
    Texture2D targetTexture = Map[arrayIdx[input.id]];

    float4 color = targetTexture.Sample(Sampler, input.Uv);
    //float4 color = Map.Sample(Sampler, float3(input.Uv, 0));
    
    return color;
}