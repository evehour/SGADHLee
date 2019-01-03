#include "000_Header.fx"


//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------
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

row_major matrix CustomView;
row_major matrix CustomProjection;
VertexOutput VS_CustomVP(VertexTexture input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, CustomView);
    output.Position = mul(output.Position, CustomProjection);

    //NDC 공간 - FVF_RHWXYZ
	//output.pPosition.xyz = output.Position.xyz / output.Position.w * 0.5f + 0.5f;
	//이후에 PS에서 UV값을 pPosition.x, pPosition.y를 사용하여 씀.
    output.Uv = input.Uv;

    return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------
Texture2D Map;

float4 PS(VertexOutput input) : SV_TARGET
{
    float4 color = Map.Sample(TrilinearSampler, input.Uv);

    return color;
}

float4 PS_TerrainHeightMapBrush(VertexOutput input) : SV_TARGET
{
    float4 color = Map.Sample(TrilinearSampler, input.Uv);
    color.rgb = float3(0, 1, 0);

    return color;
}

float4 PS_TerrainHeightMap(VertexOutput input) : SV_TARGET
{
    float4 color = Map.SampleLevel(TrilinearSampler, input.Uv, 0);
    color.r = color.r / 50.0f;

    return color;
}


//-----------------------------------------------------------------------------
// Technique
//-----------------------------------------------------------------------------
technique11 T0
{
    pass P0
    {
        SetDepthStencilState(NoDepth, 0);
        SetBlendState(AlphaBlend3, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetDepthStencilState(NoDepth, 0);
        SetBlendState(AlphaBlend3, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_TerrainHeightMapBrush()));
    }

    pass P2
    {
        SetDepthStencilState(NoDepth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_TerrainHeightMap()));
    }

    pass P3
    {
        SetDepthStencilState(NoDepth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS_CustomVP()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}