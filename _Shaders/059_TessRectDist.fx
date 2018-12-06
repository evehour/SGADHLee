#include "000_Header.fx"

float Ratio = 20.0f;
float Piece = 10.0f;

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
    float Edges[4] : SV_TessFactor; // ���� ����
    float inside[2] : SV_InsideTessFactor; // �簢���̶� �ﰢ���� 2���̹Ƿ�
};

int ComputeTesselAmount(float3 position)
{
    float dist = distance(position.xyz, ViewPosition);
    float s = saturate((dist - Ratio) / (100.0f - Ratio));

    return (int) lerp(Piece, 1, s);
}

// ��Ʈ�� ����Ʈ�� ������ ��Ȯ�� ��ġ�ؾ� ��.
// ��ġ�� 1���� ���� ������� ���� ��.
ConstantOutput HS_Constant(InputPatch<VertexOutput, 4> input, uint patchID : SV_PrimitiveID)
{
    ConstantOutput output;

    float3 center = 0;
    center = (input[0].Position + input[1].Position) * 0.5f;
    output.Edges[0] = ComputeTesselAmount(center); // ���� ��

    center = (input[1].Position + input[3].Position) * 0.5f;
    output.Edges[1] = ComputeTesselAmount(center);

    center = (input[2].Position + input[3].Position) * 0.5f;
    output.Edges[2] = ComputeTesselAmount(center);

    center = (input[0].Position + input[2].Position) * 0.5f;
    output.Edges[3] = ComputeTesselAmount(center);

    center = (input[0].Position + input[1].Position + input[2].Position + input[3].Position) * 0.25f;
    output.inside[0] = ComputeTesselAmount(center); // �̰͵� ���� ������ �ִ°� �ڿ������� �����ϴ�. ���~
    output.inside[1] = output.inside[0];

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
[domain("quad")]
//[partitioning("integer")]
//[partitioning("fractional_odd")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("HS_Constant")]
HullOutput HS(InputPatch<VertexOutput, 4> input, uint pointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
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

// �簢���� �������� 2���� uv�� �޾Ƶ� ��. �ﰢ���� 3����......
[domain("quad")]
DomainOutput DS(ConstantOutput input, float2 uv : SV_DomainLocation, const OutputPatch<HullOutput, 4> patch)
{
    DomainOutput output;
    
    // �簢���� ���̸��Ͼ�(������ �������Ѱ��� ��)
    // 3�� ����� �װͺ��� ���۵�.
    // �簢���� ������ġ ����.
    float3 v1 = lerp(patch[0].Position.xyz, patch[1].Position.xyz, 1 - uv.y);
    float3 v2 = lerp(patch[2].Position.xyz, patch[3].Position.xyz, 1 - uv.y);
    float3 position = lerp(v1, v2, uv.x);

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