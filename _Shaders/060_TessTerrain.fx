#include "000_Header.fx"

float Ratio = 2.0f;

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------
struct VertexOutput
{
    float4 Position : Position0;
    float2 Uv : Uv0;
    float3 Normal : Normal0;
};

VertexOutput VS(VertexTextureNormal input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Normal = WorldNormal(input.Normal);
    output.Uv = input.Uv;

    return output;
}

//-----------------------------------------------------------------------------
// Hull Shader
//-----------------------------------------------------------------------------
struct ConstantOutput
{
    float Edges[4] : SV_TessFactor; // 변의 갯수
    float inside[2] : SV_InsideTessFactor; // 사각형이라 삼각형이 2개이므로
};

int ComputeTesselAmount(float3 position)
{
    float dist = distance(position.xyz, ViewPosition);
    float s = saturate((dist - Ratio) / (100.0f - Ratio));

    return (int) lerp(6, 1, s);
}

// 컨트롤 포인트의 갯수와 정확히 일치해야 함.
// 패치는 1개의 도형 단위라고 보면 됨.
ConstantOutput HS_Constant(InputPatch<VertexOutput, 4> input, uint patchID : SV_PrimitiveID)
{
    ConstantOutput output;

    float3 center = 0;
    center = (input[0].Position + input[1].Position) * 0.5f;
    output.Edges[0] = ComputeTesselAmount(center); // 왼쪽 변

    center = (input[1].Position + input[3].Position) * 0.5f;
    output.Edges[1] = ComputeTesselAmount(center);

    center = (input[2].Position + input[3].Position) * 0.5f;
    output.Edges[2] = ComputeTesselAmount(center);

    center = (input[0].Position + input[2].Position) * 0.5f;
    output.Edges[3] = ComputeTesselAmount(center);

    center = (input[0].Position + input[1].Position + input[2].Position + input[3].Position) * 0.25f;
    output.inside[0] = ComputeTesselAmount(center); // 이것도 같은 값으로 주는게 자연스럽고 속편하다. 띠용~
    output.inside[1] = output.inside[0];

    return output;
}

struct HullOutput
{
    float4 Position : Position0;
    float2 Uv : Uv0;
    float3 Normal : Normal0;
};

// 삼각형 용도로 쓰겠다는 의미
// 제어점 용도로 사용하기 때문에 순서는 중요하지 않는다고 함.
// line
// line_adj
// tri
// tri_adj
[domain("quad")]
[partitioning("integer")]
//[partitioning("fractional_odd")]
//[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("HS_Constant")]
HullOutput HS(InputPatch<VertexOutput, 4> input, uint pointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HullOutput output;
    output.Position = input[pointID].Position;
    output.Uv = input[pointID].Uv;
    output.Normal = input[pointID].Normal;

    return output;
}


//-----------------------------------------------------------------------------
// Domain Shader
//-----------------------------------------------------------------------------
struct DomainOutput
{
    float4 Position : SV_Position0;
    float2 Uv : Uv0;
    float3 Normal : Normal0;
};

// 사각형은 제어점이 2개라 uv로 받아도 됨. 삼각형은 3갠데......
[domain("quad")]
DomainOutput DS(ConstantOutput input, float2 uv : SV_DomainLocation, const OutputPatch<HullOutput, 4> patch)
{
    DomainOutput output;
    
    // 사각형은 바이리니어(러프를 누적시켜가는 것)
    // 3차 베지어도 그것부터 시작됨.
    // 사각형의 정점위치 계산식.
    float3 v1 = lerp(patch[0].Position.xyz, patch[1].Position.xyz, 1 - uv.y);
    float3 v2 = lerp(patch[2].Position.xyz, patch[3].Position.xyz, 1 - uv.y);
    float3 position = lerp(v1, v2, uv.x);
    
    output.Position = mul(float4(position, 1), View);
    output.Position = mul(output.Position, Projection);
    
    
    float2 uv1 = lerp(patch[0].Uv.xy, patch[1].Uv.xy, 1 - uv.y);
    float2 uv2 = lerp(patch[2].Uv.xy, patch[3].Uv.xy, 1 - uv.y);
    output.Uv = lerp(uv1, uv2, uv.x);
    //output.Uv.x = lerp(patch[1].Uv.x, patch[3].Uv.x, uv.x); // 좌상 -> 우상
    //output.Uv.y = lerp(patch[1].Uv.y, patch[0].Uv.y, 1 - uv.y); // 좌상 -> 좌하

    float3 n1 = lerp(patch[0].Normal.xyz, patch[1].Normal.xyz, 1 - uv.y);
    float3 n2 = lerp(patch[2].Normal.xyz, patch[3].Normal.xyz, 1 - uv.y);
    output.Normal = lerp(n1, n2, uv.x);

    return output;
}


//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------
float4 PS(DomainOutput input) : SV_TARGET
{
    float4 color = 0;
    float4 diffuse = DiffuseMap.Sample(TrilinearSampler, input.Uv);
    float4 NdotL = dot(-LightDirection, normalize(input.Normal));
    
    //color = NdotL * diffuse;
    color = diffuse;

    return color;
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