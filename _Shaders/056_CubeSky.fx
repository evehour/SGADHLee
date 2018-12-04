#include "000_Header.fx"

struct VertexOutput
{
    float4 Position : SV_POSITION0;
    float4 oPosition : POSITION1;
    float3 Normal : NORMAL0;
};

VertexOutput VS(VertexTextureNormalTangent input)
{
    VertexOutput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.oPosition = input.Position;

    output.Normal = input.Normal;

    return output;
}

///////////////////////////////////////////////////////////////////////////////

TextureCube CubeMap;

float4 PS(VertexOutput input) : SV_TARGET
{
    // input.oPosition.xyz : 큐브맵에서의 이 값은 방향
    //      해당 방향으로 충돌하는 픽셀에 그리는 것.
    //      비용이 상당함.
    return CubeMap.Sample(TrilinearSampler, input.oPosition.xyz);
}

DepthStencilState Depth
{
    //DepthEnable = false;
    DepthFunc = LESS_EQUAL;
};

RasterizerState Cull
{
    FrontCounterClockwise = true;
};

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));

        SetDepthStencilState(Depth, 0);
        SetRasterizerState(Cull);
    }
}