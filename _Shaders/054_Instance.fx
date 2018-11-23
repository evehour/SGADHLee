#include "000_Header.fx"


struct VertexInput
{
    float4 Position : POSITION0;
    float2 Uv : UV0;

    matrix World : INSTANCE0;
    uint Texture : INSTANCE4;

    uint InstanceID : SV_INSTANCEID0;
};

struct VertexOutput
{
    float4 Position : SV_POSITION0;
    float2 Uv : UV0;

    uint Texture : TEXTURE0;
    uint id : INSTANCEID0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Position = mul(input.Position, input.World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;
    output.Texture = input.Texture;
    output.id = input.InstanceID;

    return output;
}

//Texture2D Map[6] : register(t10);
Texture2DArray Map;

float4 PS(VertexOutput input) : SV_TARGET
{
    float4 color = Map.Sample(TrilinearSampler, float3(input.Uv, input.Texture));
    
    return color;
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}
