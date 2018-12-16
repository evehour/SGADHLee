#include "000_Header.fx"

cbuffer CB_Terrain
{
    float4 FogColor;
    float FogStart;
    float FogRange;

    float MinDistance;
    float MaxDistance;
    float MinTessellation;
    float MaxTessellation;

    float TexelCellSpaceU;
    float TexelCellSpaceV;
    float WorldCellSpace;
   
    float2 TexScale = 66.0f;
    float CB_Terrain_Padding;

    float4 WorldFrustumPlanes[6];
};

Texture2D HeightMap;
SamplerState HeightMapSampler
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
};

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

struct VertexInput
{
    float4 Position : Position0;
    float2 Uv : Uv0;
    float2 BoundsY : BoundsY0;
};

struct VertexOutput
{
    float4 Position : Position0;
    float2 Uv : Uv0;
    float2 BoundsY : BoundsY0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = input.Position;
   // sample�� �ص� �Ǵµ� mipmap ������ ����Ϸ��� sample level ����
    output.Position.y = HeightMap.SampleLevel(HeightMapSampler, input.Uv, 0).r; 

    output.Uv = input.Uv;
    output.BoundsY = input.BoundsY;

    return output;
}

//-----------------------------------------------------------------------------
// Hull Shader
//-----------------------------------------------------------------------------

float CalcTessFactor(float3 position)
{
    float d = distance(position, ViewPosition);

    float s = saturate((d - MinDistance) / (MaxDistance - MinDistance));

    return pow(2, lerp(MaxTessellation, MinTessellation, s));
}

bool AabbBehindPlaneTest(float3 center, float3 extents, float4 plane)
{
    float3 n = abs(plane.xyz);
   
    float r = dot(extents, n);

    float s = dot(float4(center, 1.0f), plane);

    return (s + r) < 0.0f;
}

bool AabbOutsideFrustumTest(float3 center, float3 extents)
{
   [unroll]
    for (int i = 0; i < 6; i++)
    {
      [flatten]
        if (AabbBehindPlaneTest(center, extents, WorldFrustumPlanes[i]))
        {
            return true;
        }
    }

    return false;
}

struct ConstantOutput
{
    float Edges[4] : SV_TessFactor; 
    float Inside[2] : SV_InsideTessFactor; 
};

ConstantOutput HS_Constant(InputPatch<VertexOutput, 4> input, uint patchID : SV_PrimitiveId)
{
    ConstantOutput output;

    float minY = input[0].BoundsY.x;
    float maxY = input[0].BoundsY.y;

    float3 vMin = float3(input[2].Position.x, minY, input[2].Position.z);
    float3 vMax = float3(input[1].Position.x, maxY, input[1].Position.z);

    float3 boxCenter = 0.5f * (vMin + vMax);
    float3 boxExtents = 0.5f * (vMax - vMin);

   [flatten]
    if (AabbOutsideFrustumTest(boxCenter, boxExtents))
    {
        output.Edges[0] = 0.0f;
        output.Edges[1] = 0.0f;
        output.Edges[2] = 0.0f;
        output.Edges[3] = 0.0f;

        output.Inside[0] = 0.0f;
        output.Inside[1] = 0.0f;

        return output;
    }

    float3 e0 = (input[0].Position + input[2].Position).xyz * 0.5f;
    float3 e1 = (input[0].Position + input[1].Position).xyz * 0.5f;
    float3 e2 = (input[1].Position + input[3].Position).xyz * 0.5f;
    float3 e3 = (input[2].Position + input[3].Position).xyz * 0.5f;

    float3 c = 0.25f * 
      (input[0].Position + input[1].Position + input[2].Position + input[3].Position).xyz;

    output.Edges[0] = CalcTessFactor(e0);
    output.Edges[1] = CalcTessFactor(e1);
    output.Edges[2] = CalcTessFactor(e2);
    output.Edges[3] = CalcTessFactor(e3);

    output.Inside[0] = CalcTessFactor(c);
    output.Inside[1] = output.Inside[0];

    return output;
}

struct HullOutput
{
    float4 Position : Position0;
    float2 Uv : Uv0;
};

[domain("quad")] // �ﰢ�� �뵵�� ���ڴٴ°�
// line, line-adj, tri, tri-adj ��� ���� HS ��� ġ�� ms �޴��� ���� // adj �� ���� �� 4�� ���� ������ �� ����ó��
[partitioning("fractional_even")] // �߶󳻴� ������ int���� �� �� ���� float�̸� ���õ� float ���� ���������µ� �ӵ��� ������
//[partitioning("fractional_odd")] // �߶󳻴� ������ int���� �� �� ���� float�̸� ���õ� float ���� ���������µ� �ӵ��� ������
// ��Ը� �������� �̵��� �������� ��Ÿ�� ������ �� ������
[outputtopology("triangle_cw")] // ������ � �������� �������� �׻� �ð�������� �����
[outputcontrolpoints(4)] // ��Ʈ�� ������ � ������ ������
[patchconstantfunc("HS_Constant")] // ��� �� ��� hull shader �̸��� ����

// ���� �ӵ��� �ö󰣴ٰ� �� �Ƚᵵ �Ǵµ� �̸� 64���� ������ ����� �����ٰ� �� �ƴϸ� ������ ������ �����Ǽ� �ȴٰ���
[maxtessfactor(64.0f)] // ���� ���� �ִ� tess factor �ִ� 32 * 2 ���� ���� ����ȭ ���߱� ���� 64���� ����ٰ� ��

HullOutput HS(InputPatch<VertexOutput, 4> input, uint pointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HullOutput output;
    output.Position = input[pointID].Position;
    output.Uv = input[pointID].Uv;

    return output;
}

//-----------------------------------------------------------------------------
// Domain Shader
//-----------------------------------------------------------------------------

struct DomainOutput
{
    float4 Position : SV_Position0;
    float3 wPosition : Position1;
    float2 Uv : Uv0;
    float2 TileUv : Uv1;
};

[domain("quad")]
// �簢���� �������� 2���� float2 uv�� �޾Ƶ� �� �ﰢ���� float3�� �޾ƾ���
DomainOutput DS(ConstantOutput input, float2 uvw : SV_DomainLocation, const OutputPatch<HullOutput, 4> patch)
{
    DomainOutput output;

    float3 p0 = lerp(patch[0].Position, patch[1].Position, uvw.x).xyz;
    float3 p1 = lerp(patch[2].Position, patch[3].Position, uvw.x).xyz;
    float3 position = lerp(p0, p1, uvw.y);
    output.wPosition = position;

    float2 uv0 = lerp(patch[0].Uv, patch[1].Uv, uvw.x);
    float2 uv1 = lerp(patch[2].Uv, patch[3].Uv, uvw.x);
   output.Uv = lerp(uv0, uv1, uvw.y);

   output.TileUv = output.Uv * TexScale;

    output.wPosition.y = HeightMap.SampleLevel(HeightMapSampler, output.Uv, 0).r;

    output.Position = mul(float4(output.wPosition, 1), World);
    output.Position = mul(float4(output.wPosition, 1), View);
    output.Position = mul(output.Position, Projection);

    return output;
}

//-----------------------------------------------------------------------------
// Pisxel Shader
//-----------------------------------------------------------------------------

Texture2DArray LayerMapArray;
Texture2D BlendMap;

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PS(DomainOutput input, uniform bool fogEnabled) : SV_TARGET
{
    float2 left = input.Uv + float2(-TexelCellSpaceU, 0.0f);
    float2 right = input.Uv + float2(TexelCellSpaceU, 0.0f);
    float2 top = input.Uv + float2(0.0f, -TexelCellSpaceV);
    float2 bottom = input.Uv + float2(0.0f, TexelCellSpaceV);

    float leftY = HeightMap.SampleLevel(HeightMapSampler, left, 0).r;
    float rightY = HeightMap.SampleLevel(HeightMapSampler, right, 0).r;
    float bottomY = HeightMap.SampleLevel(HeightMapSampler, top, 0).r;
    float topY = HeightMap.SampleLevel(HeightMapSampler, bottom, 0).r;

    float3 tangent = normalize(float3(2.0f * WorldCellSpace, rightY - leftY, 0.0f));
    float3 biTangent = normalize(float3(0.0f, bottomY - topY, -2.0f * WorldCellSpace));
    float3 normalW = cross(tangent, biTangent);

    float3 eye = ViewPosition - input.wPosition;
    float distanceToEye = length(eye);
    eye /= distanceToEye;

 //   float4 ambient = float4(0, 0, 0, 0);
 //   float4 diffuse = float4(0, 0, 0, 0);
 //   float4 specular = float4(0, 0, 0, 0);

 //   float4 A, D, S;
   //ComputeDirectionalLight(

    float4 c0 = LayerMapArray.Sample(LinearSampler, float3(input.TileUv, 0));
    float4 c1 = LayerMapArray.Sample(LinearSampler, float3(input.TileUv, 1));
    float4 c2 = LayerMapArray.Sample(LinearSampler, float3(input.TileUv, 2));
    float4 c3 = LayerMapArray.Sample(LinearSampler, float3(input.TileUv, 3));
    float4 c4 = LayerMapArray.Sample(LinearSampler, float3(input.TileUv, 4));

    float4 t = BlendMap.Sample(LinearSampler, input.Uv);

    float4 color = c0;
    color = lerp(color, c1, t.r);
    color = lerp(color, c2, t.g);
    color = lerp(color, c3, t.b);
    color = lerp(color, c4, t.a);

    [flatten]
    if (fogEnabled == true)
    {
        float fogFactor = saturate((distanceToEye - FogStart) / FogRange);

        color = lerp(color, FogColor, fogFactor);
    }

    return color;
}

//-----------------------------------------------------------------------------
// States
//-----------------------------------------------------------------------------

RasterizerState FillMode
{
    FillMode = Wireframe;
};

//-----------------------------------------------------------------------------
// Techinques
//-----------------------------------------------------------------------------

technique11 T0
{
    pass P0
    {
        SetRasterizerState(NULL);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetHullShader(CompileShader(hs_5_0, HS()));
        SetDomainShader(CompileShader(ds_5_0, DS()));
        SetPixelShader(CompileShader(ps_5_0, PS(true)));
    }

    pass P1
    {
        SetRasterizerState(FillMode);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetHullShader(CompileShader(hs_5_0, HS()));
        SetDomainShader(CompileShader(ds_5_0, DS()));
        SetPixelShader(CompileShader(ps_5_0, PS(true)));
    }
}