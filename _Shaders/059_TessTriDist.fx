#include "000_Header.fx"

float Ratio = 20.0f;

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------
struct VertexOutput
{
    float4 Position : Position0;
};

VertexOutput VS(Vertex input)
{
    VertexOutput output;
    output.Position = input.Position;

    return output;
}

//-----------------------------------------------------------------------------
// Hull Shader
//-----------------------------------------------------------------------------
struct ConstantOutput
{
    float Edges[3] : SV_TessFactor; // ���� ����
    float inside : SV_InsideTessFactor; // �ﰢ�� ����
};

int ComputeTesselAmount(float3 position)
{
    float dist = distance(position.xyz, ViewPosition);
    float s = saturate((dist - Ratio) / (100.0f - Ratio));

    return (int) lerp(64, 1, s);
}

// ��Ʈ�� ����Ʈ�� ������ ��Ȯ�� ��ġ�ؾ� ��.
// ��ġ�� 1���� ���� ������� ���� ��.
ConstantOutput HS_Constant(InputPatch<VertexOutput, 3> input, uint patchID : SV_PrimitiveID)
{
    ConstantOutput output;

    // �߽��� ��� ���. �� ������ �¾ҳ�
    float3 center = 0.33f * (input[0].Position + input[1].Position + input[2].Position);
    float3 position = mul(float4(center, 1), World).xyz;
    int factor = ComputeTesselAmount(position);
    output.Edges[0] = factor;
    output.Edges[1] = factor;
    output.Edges[2] = factor;

    output.inside = factor; // �̰͵� ���� ������ �ִ°� �ڿ������� �����ϴ�. ���~

    return output;
}

struct HullOutput
{
    float4 Position : Position0;
};

// �ﰢ�� �뵵�� ���ڴٴ� �ǹ�
// ������ �뵵�� ����ϱ� ������ ������ �߿����� �ʴ´ٰ� ��.
// line
// line_adj
// tri
// tri_adj
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HS_Constant")]
HullOutput HS(InputPatch<VertexOutput, 3> input, uint pointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HullOutput output;
    output.Position = input[pointID].Position;

    return output;
}


//-----------------------------------------------------------------------------
// Domain Shader
//-----------------------------------------------------------------------------
struct DomainOutput
{
    float4 Position : SV_Position0;
};

[domain("tri")]
DomainOutput DS(ConstantOutput input, float3 uvw : SV_DomainLocation, const OutputPatch<HullOutput, 3> patch)
{
    DomainOutput output;

    float3 position = uvw.x * patch[0].Position + uvw.y * patch[1].Position + uvw.z * patch[2].Position;
    position.y = 0.3f * (position.y * sin(position.x) + position.x * cos(position.y));

    output.Position = mul(float4(position, 1), World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    return output;
}


//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------
float4 PS(DomainOutput input) : SV_TARGET
{
    return float4(0, 0, 1, 1);
}



//-----------------------------------------------------------------------------
// Technique
//-----------------------------------------------------------------------------



technique11 T0
{
    pass P0
    {
        SetRasterizerState(WireNoCullNoAA);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetHullShader(CompileShader(hs_5_0, HS()));
        SetDomainShader(CompileShader(ds_5_0, DS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}