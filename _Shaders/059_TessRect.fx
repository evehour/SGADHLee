#include "000_Header.fx"

int TessAmount = 1;
int TessAmountInside = 1;
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
    float Edges[4] : SV_TessFactor; // ���� ����
    float inside[2] : SV_InsideTessFactor; // �簢���̶� �ﰢ���� 2���̹Ƿ�
};

int ComputeTesselAmount(float3 position)
{
    float dist = distance(position.xyz, ViewPosition);
    float s = saturate((dist - Ratio) / (100.0f - Ratio));

    return (int) lerp(64, 1, s);
}

// ��Ʈ�� ����Ʈ�� ������ ��Ȯ�� ��ġ�ؾ� ��.
// ��ġ�� 1���� ���� ������� ���� ��.
ConstantOutput HS_Constant(InputPatch<VertexOutput, 4> input, uint patchID : SV_PrimitiveID)
{
    ConstantOutput output;

    //// �߽��� ��� ���. �� ������ �¾ҳ�
    //float3 center = 0.33f * (input[0].Position + input[1].Position + input[2].Position);
    //float3 position = mul(float4(center, 1), World).xyz;
    //int factor = ComputeTesselAmount(position);
    output.Edges[0] = TessAmount;
    output.Edges[1] = TessAmount;
    output.Edges[2] = TessAmount;
    output.Edges[3] = TessAmount;

    output.inside[0] = TessAmountInside; // �̰͵� ���� ������ �ִ°� �ڿ������� �����ϴ�. ���~
    output.inside[1] = TessAmountInside;

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
[partitioning("integer")]
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