#include "000_Header.fx"

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};

VertexOutput VS(VertexTexture input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    //NDC 공간 - FVF_RHWXYZ
	//output.pPosition.xyz = output.Position.xyz / output.Position.w * 0.5f + 0.5f;
	//이후에 PS에서 UV값을 pPosition.x, pPosition.y를 사용하여 씀.
    output.Uv = input.Uv;

    return output;
}


Texture2D Map;

float4 PS(VertexOutput input) : SV_TARGET
{
    float4 color = Map.Sample(TrilinearSampler, input.Uv);

    return color;
}



technique11 T0
{
    pass P0
    {
        SetDepthStencilState(NoDepth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}