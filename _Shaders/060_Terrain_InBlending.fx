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

float2 HeightMapSize; // HeightMap 가로/세로 크기
float2 TerrainSize; // 지형맵 버텍스 실제 위치의 크기

float MaxHeight;
float4 BlendPositionRate;
float2 BlendMinMax[4];

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
   // sample로 해도 되는데 mipmap 잇으면 고려하려고 sample level 쓴거
    output.Position.y = HeightMap.SampleLevel(HeightMapSampler, input.Uv, 0).r;
    output.Position.w = 1;
     
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
    for (int i = 0; i < 6; i++)
    {
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

    //float minY = input[0].Position.y;
    //float maxY = input[0].Position.y;
    //maxY = 1.175494351e-38F;
    //minY = 3.402823466e+38F;
    
    //float2 c[4];
    //c[1].x = input[0].Position.x;
    //c[1].y = input[0].Position.z + (0.5f * (HeightMapSize.y * 0.5f)) / 0.5f;
    //c[2].x = input[0].Position.x + (0.5f * (HeightMapSize.x * 0.5f)) / 0.5f;
    //c[2].y = input[0].Position.z;
    //c[3].x = input[0].Position.x + (0.5f * (HeightMapSize.x * 0.5f)) / 0.5f;
    //c[3].y = (input[0].Position.z + 0.5f * (HeightMapSize.y * 0.5f)) / 0.5f;

    //c[0] = input[0].Uv;
    //c[1] = (c[1] + 512.0f) / 1024.0f;
    //c[2] = (c[2] + 512.0f) / 1024.0f;
    //c[3] = (c[3] + 512.0f) / 1024.0f;

    //float h[4];
    //h[0] = input[0].Position.y;
    //h[1] = HeightMap.SampleLevel(HeightMapSampler, c[1], 0).r;
    //h[2] = HeightMap.SampleLevel(HeightMapSampler, c[2], 0).r;
    //h[3] = HeightMap.SampleLevel(HeightMapSampler, c[3], 0).r;
    

    //for (int ii = 0; ii < 4; ++ii)
    //{
    //    minY = min(minY, h[ii]);
    //    maxY = max(maxY, h[ii]);
    //}

    float minY = input[0].BoundsY.x;
    float maxY = input[0].BoundsY.y;

    maxY = 1.175494351e-38F;
    minY = 3.402823466e+38F;

    for (int ii = 0; ii < 4; ++ii)
    {
        minY = min(minY, input[ii].Position.y);
        maxY = max(maxY, input[ii].Position.y);
    }
    
    float3 vMin = float3(input[2].Position.x, minY, input[2].Position.z);
    float3 vMax = float3(input[1].Position.x, maxY, input[1].Position.z);

    float3 boxCenter = 0.5f * (vMin + vMax);
    float3 boxExtents = 0.5f * (vMax - vMin);
    
    if (AabbOutsideFrustumTest(boxCenter, boxExtents))
    {
        output.Edges[0] = 0.0f;
        output.Edges[1] = 0.0f;
        output.Edges[2] = 0.0f;
        output.Edges[3] = 0.0f;

        output.Inside[0] = 0.0f;
        output.Inside[1] = 0.0f;
        
    }
    else
    {
        float3 e0 = (input[0].Position + input[2].Position).xyz * 0.5f;
        float3 e1 = (input[0].Position + input[1].Position).xyz * 0.5f;
        float3 e2 = (input[1].Position + input[3].Position).xyz * 0.5f;
        float3 e3 = (input[2].Position + input[3].Position).xyz * 0.5f;

        float3 c = 0.25f * (input[0].Position + input[1].Position + input[2].Position + input[3].Position).xyz;

        output.Edges[0] = CalcTessFactor(e0);
        output.Edges[1] = CalcTessFactor(e1);
        output.Edges[2] = CalcTessFactor(e2);
        output.Edges[3] = CalcTessFactor(e3);

        output.Inside[0] = CalcTessFactor(c);
        output.Inside[1] = output.Inside[0];
    }

    return output;
}

struct HullOutput
{
    float4 Position : Position0;
    float2 Uv : Uv0;
};

[domain("quad")] // 삼각형 용도로 쓰겠다는거
// line, line-adj, tri, tri-adj 등등 있음 HS 라고 치면 ms 메뉴얼 나옴 // adj 는 인접 점 4개 들어가면 띄엄띄엄 들어감 점선처럼
[partitioning("fractional_even")] // 잘라내는 기준이 int형만 들어갈 수 있음 float이면 무시됨 float 쓰면 정밀해지는데 속도는 느려짐
//[partitioning("fractional_odd")] // 잘라내는 기준이 int형만 들어갈 수 있음 float이면 무시됨 float 쓰면 정밀해지는데 속도는 느려짐
// 대규모 지형에서 이동시 팝핑현상 나타는 이유가 이 때문임
[outputtopology("triangle_cw")] // 정점을 어떤 방향으로 감을껀지 항상 시계방향으로 써왔음
[outputcontrolpoints(4)] // 컨트롤 포인터 몇개 단위로 보낼지
[patchconstantfunc("HS_Constant")] // 사용 할 상수 hull shader 이름이 뭔지

// 쓰면 속도가 올라간다고 함 안써도 되는데 미리 64개의 공간을 만들어 놓은다고 함 아니면 들어오는 순서에 누적되서 된다고함
[maxtessfactor(64.0f)] // 내가 써줄 최대 tess factor 최대 32 * 2 까지 가능 최적화 맞추기 위해 64개로 맞춘다고 함

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
    float4 dPosition : Position2;
    float4 ddPosition : Position3;
    float2 Uv : Uv0;
    float2 TileUv : Uv1;
};

[domain("quad")]
// 사각형은 제어점이 2개라 float2 uv로 받아도 됨 삼각형은 float3로 받아야함
DomainOutput DS(ConstantOutput input, float2 uv : SV_DomainLocation, const OutputPatch<HullOutput, 4> patch, uniform bool bDepthDraw)
{
    DomainOutput output;

    float3 p0 = lerp(patch[0].Position, patch[1].Position, uv.x).xyz;
    float3 p1 = lerp(patch[2].Position, patch[3].Position, uv.x).xyz;
    float3 position = lerp(p0, p1, uv.y);
    output.wPosition = position;
    output.wPosition = mul(float4(output.wPosition, 1), World).xyz;

    float2 uv0 = lerp(patch[0].Uv, patch[1].Uv, uv.x);
    float2 uv1 = lerp(patch[2].Uv, patch[3].Uv, uv.x);
    output.Uv = lerp(uv0, uv1, uv.y);

    output.TileUv = output.Uv * TexScale;

    output.wPosition.y = HeightMap.SampleLevel(HeightMapSampler, output.Uv, 0).r;

    output.dPosition = mul(float4(output.wPosition, 1), LightViewMatrix);
    output.dPosition = mul(output.dPosition, ShadowProjection);
    
    if(bDepthDraw)
    {
        output.Position = output.dPosition;
    }
    else
    {
        output.Position = mul(float4(output.wPosition, 1), View);
        output.Position = mul(output.Position, Projection);
    }
    output.ddPosition = output.Position;

    return output;
}

//-----------------------------------------------------------------------------
// Pisxel Shader
//-----------------------------------------------------------------------------

Texture2DArray LayerMapArray;
Texture2DArray LayerNormalMapArray;
Texture2D BlendMap;

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Wrap;
    AddressV = Wrap;
};

struct PixelTargetOutput
{
    float4 tColor : SV_TARGET0;
    float4 pColor : SV_TARGET1;
};

float4 BlendLerpHelper(float4 fColor, float4 sColor, float wPosRate, float4 BPR, float minusRate, float plusRate)
{
    float4 rColor = fColor;
    
    rColor = (BPR < 0.01f && BPR > -0.001f) ? lerp(fColor, sColor, saturate((0.01f - wPosRate) / (0.01f + plusRate - minusRate))) : lerp(fColor, sColor, saturate((wPosRate - (BPR - minusRate)) / ((BPR + plusRate) - (BPR - minusRate))));

    return rColor;
}

float4 BlendLerpHelper2(float4 fColor, float4 sColor, float wPosRate, float4 BPR, float minusRate, float plusRate)
{
    float4 rColor = 0;

    rColor = lerp(fColor, sColor, saturate((wPosRate - (BPR - minusRate)) / ((BPR + plusRate) - (BPR - minusRate))));

    rColor = (BPR < 0.01f && wPosRate < 0.01f) ? sColor : rColor;

    return rColor;
}

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
    
    float4 color = c0;
    float pRate = input.wPosition.y / MaxHeight;
    
    //color = lerp(color, c1, saturate((pRate - (BlendPositionRate[0] - 0.05f)) / ((BlendPositionRate[0] + 0.05f) - (BlendPositionRate[0] - 0.05f))));
    //color = lerp(color, c2, saturate((pRate - (BlendPositionRate[1] - 0.15f)) / ((BlendPositionRate[1] + 0.15f) - (BlendPositionRate[1] - 0.15f))));
    //color = lerp(color, c4, saturate((pRate - (BlendPositionRate[3] - 0.10f)) / ((BlendPositionRate[3] + 0.10f) - (BlendPositionRate[3] - 0.10f))));
    
    //color = BlendLerpHelper(color, c1, pRate, BlendPositionRate[0], 0.05f, 0.05f);
    //color = BlendLerpHelper(color, c2, pRate, BlendPositionRate[1], 0.15f, 0.15f);
    //color = BlendLerpHelper(color, c3, pRate, BlendPositionRate[2], 0.05f, 0.05f);
    //color = BlendLerpHelper(color, c4, pRate, BlendPositionRate[3], 0.10f, 0.10f);
    
    color = BlendLerpHelper(color, c1, pRate, BlendPositionRate[0], 0.05f, 0.05f);
    color = BlendLerpHelper(color, c2, pRate, BlendPositionRate[1], 0.15f, 0.15f);
    color = BlendLerpHelper(color, c3, pRate, BlendPositionRate[2], 0.01f, 0.01f);
    color = BlendLerpHelper(color, c4, pRate, BlendPositionRate[3], 0.05f, 0.05f);

    [flatten]
    if (fogEnabled == true)
    {
        float fogFactor = saturate((distanceToEye - FogStart) / FogRange);

        color = lerp(color, FogColor, fogFactor);
    }
    
    return color;
}

PixelTargetOutput PSMR(DomainOutput input, uniform bool fogEnabled) : SV_TARGET
{
    PixelTargetOutput output;

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
    
    float4 color = c0;
    float pRate = input.wPosition.y / MaxHeight;
    
    float4 blendColor;
    blendColor = BlendLerpHelper(color, c1, pRate, BlendPositionRate[0], BlendMinMax[0].x, BlendMinMax[0].y);
    color = length(blendColor) > 0 ? blendColor : color;

    blendColor = BlendLerpHelper(color, c2, pRate, BlendPositionRate[1], BlendMinMax[1].x, BlendMinMax[1].y);
    color = length(blendColor) > 0 ? blendColor : color;

    blendColor = BlendLerpHelper(color, c3, pRate, BlendPositionRate[2], BlendMinMax[2].x, BlendMinMax[2].y);
    color = length(blendColor) > 0 ? blendColor : color;

    blendColor = BlendLerpHelper(color, c4, pRate, BlendPositionRate[3], BlendMinMax[3].x, BlendMinMax[3].y);
    color = length(blendColor) > 0 ? blendColor : color;

    [flatten]
    if (fogEnabled == true)
    {
        float fogFactor = saturate((distanceToEye - FogStart) / FogRange);

        color = lerp(color, FogColor, fogFactor);
    }

    output.tColor = color;
    output.pColor = float4(input.Uv.x, abs(1 - input.Uv.y), 0, 1);
    return output;
}


//-----------------------------------------------------------------------------

Texture2D BrushTexture;
bool IsHovering = false;
float2 PickPosition = float2(0, 0);
float2 BrushRate = float2(1, 1);

float4 BrushHelper(float4 color, float2 uv, Texture2D brushTexture)
{
    float4 rColor = color;

    float2 mousePos = PickPosition / HeightMapSize;
    mousePos.y = 1.0f - mousePos.y; // 픽포지션 변환작업으로 반전되어있어서 다시 되돌려줘야함.

    float2 pix1uv = 1.0f / TerrainSize;
    float2 brushAreaMinUv = mousePos - pix1uv * (BrushRate * 0.25f);
    float2 brushAreaMaxUv = mousePos + pix1uv * (BrushRate * 0.25f);

    if (
        (uv.x > brushAreaMinUv.x) && (uv.x < brushAreaMaxUv.x)
        && (uv.y > brushAreaMinUv.y) && (uv.y < brushAreaMaxUv.y)
        )
    {
        float2 brushUv = (uv - brushAreaMinUv) / (brushAreaMaxUv - brushAreaMinUv);
        float4 brushColor = brushTexture.Sample(TrilinearSampler, brushUv);
        
        brushColor.a = (brushColor.r > 0.99 && brushColor.g < 0.01f && brushColor.b > 0.99) ? 0 : brushColor.a;
        rColor.xyz += brushColor.a > 0.01f ? float3(0, 0.25f, 0) : 0;

    }

    return rColor;
}

float4 BrushHelper_origin(float4 color, float2 uv, Texture2D brushTexture)
{
    float4 rColor = color;
    float2 mousePos = PickPosition;
    mousePos.y = 1.0f - mousePos.y;
    mousePos /= 2048.0f;

    float2 pix1uv = 1.0f / 1024.0f;
    float2 brushAreaMinUv = mousePos - pix1uv * (BrushRate * 0.5f);
    float2 brushAreaMaxUv = mousePos + pix1uv * (BrushRate * 0.5f);

    if (
        (uv.x > brushAreaMinUv.x) && (uv.x < brushAreaMaxUv.x)
        && (uv.y > brushAreaMinUv.y) && (uv.y < brushAreaMaxUv.y)
        )
    {
        float2 brushUv = uv - brushAreaMinUv / (brushAreaMaxUv - brushAreaMinUv);
        float4 brushColor = brushTexture.Sample(TrilinearSampler, brushUv);
        rColor.xyz = brushColor.xyz;
        rColor = 1;
    }

    return rColor;
}

Texture2D ShadowMap;
SamplerComparisonState DepthCompSampler
{
    Filter = COMPARISON_MIN_MAG_MIP_LINEAR;

    ComparisonFunc = LESS_EQUAL;
    AddressU = Mirror;
    AddressV = Mirror;
    MaxAnisotropy = 1;
};

SamplerComparisonState samShadow
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
    BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    ComparisonFunc = LESS;
};

PixelTargetOutput PSMRBrush(DomainOutput input, uniform bool fogEnabled) : SV_TARGET
{
    PixelTargetOutput output;

    float detail = input.ddPosition.z / input.ddPosition.w;
    float2 left = input.Uv + float2(-TexelCellSpaceU, 0.0f);
    float2 right = input.Uv + float2(TexelCellSpaceU, 0.0f);
    float2 top = input.Uv + float2(0.0f, -TexelCellSpaceV);
    float2 bottom = input.Uv + float2(0.0f, TexelCellSpaceV);

    float leftY = HeightMap.SampleLevel(HeightMapSampler, left, 0).r;
    float rightY = HeightMap.SampleLevel(HeightMapSampler, right, 0).r;
    float bottomY = HeightMap.SampleLevel(HeightMapSampler, top, 0).r;
    float topY = HeightMap.SampleLevel(HeightMapSampler, bottom, 0).r;

    // https://www.slideshare.net/QooJuice/normal-mapping-79468987 참조.
    float3 tangent = normalize(float3(2.0f * WorldCellSpace, rightY - leftY, 0.0f));
    float3 biTangent = normalize(float3(0.0f, bottomY - topY, -2.0f * WorldCellSpace));
    float3 normalW = cross(tangent, biTangent);
    float3 biNormalW = cross(tangent, normalW);
    float3 normalF = normalW;

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
    
    float4 n0 = LayerNormalMapArray.Sample(LinearSampler, float3(input.TileUv, 0));
    float4 n1 = LayerNormalMapArray.Sample(LinearSampler, float3(input.TileUv, 1));
    float4 n2 = LayerNormalMapArray.Sample(LinearSampler, float3(input.TileUv, 2));
    float4 n3 = LayerNormalMapArray.Sample(LinearSampler, float3(input.TileUv, 3));
    float4 n4 = LayerNormalMapArray.Sample(LinearSampler, float3(input.TileUv, 4));
    
    float4 color = c0;
    float4 nColor = n0;
    float pRate = input.wPosition.y / MaxHeight;
    
    float4 blendColor, blendNormalColor;
    blendColor = BlendLerpHelper(color, c1, pRate, BlendPositionRate[0], BlendMinMax[0].x, BlendMinMax[0].y);
    color = length(blendColor) > 0 ? blendColor : color;

    blendColor = BlendLerpHelper(color, c2, pRate, BlendPositionRate[1], BlendMinMax[1].x, BlendMinMax[1].y);
    color = length(blendColor) > 0 ? blendColor : color;

    blendColor = BlendLerpHelper(color, c3, pRate, BlendPositionRate[2], BlendMinMax[2].x, BlendMinMax[2].y);
    color = length(blendColor) > 0 ? blendColor : color;

    blendColor = BlendLerpHelper(color, c4, pRate, BlendPositionRate[3], BlendMinMax[3].x, BlendMinMax[3].y);
    color = length(blendColor) > 0 ? blendColor : color;
    
    if(detail < 0.998f)
    {
        blendNormalColor = BlendLerpHelper(nColor, n1, pRate, BlendPositionRate[0], BlendMinMax[0].x, BlendMinMax[0].y);
        nColor = length(blendNormalColor) > 0 ? blendNormalColor : nColor;
        blendNormalColor = BlendLerpHelper(nColor, n2, pRate, BlendPositionRate[1], BlendMinMax[1].x, BlendMinMax[1].y);
        nColor = length(blendNormalColor) > 0 ? blendNormalColor : nColor;
        blendNormalColor = BlendLerpHelper(nColor, n3, pRate, BlendPositionRate[2], BlendMinMax[2].x, BlendMinMax[2].y);
        nColor = length(blendNormalColor) > 0 ? blendNormalColor : nColor;
        blendNormalColor = BlendLerpHelper(nColor, n4, pRate, BlendPositionRate[3], BlendMinMax[3].x, BlendMinMax[3].y);
        nColor = length(blendNormalColor) > 0 ? blendNormalColor : nColor;

        float3x3 TBN = float3x3(tangent, biTangent, normalW);
        float3 coord = 2.f * nColor.xyz - 1.0f;
        normalF = mul(coord, TBN);
    }

    color *= dot(-LightDirection, normalize(normalF));

    if (detail < 0.998f && (LightDirection.y) <= 0.2)
    {
        float depth = 0;
        float3 vPosition = input.dPosition.xyz / input.dPosition.w;

        if (!(vPosition.x < -1.0f || vPosition.x > 1.0f ||
            vPosition.y < -1.0f || vPosition.y > 1.0f ||
            vPosition.z < 0.0f || vPosition.z > 1.0f))
        {
            vPosition.x = vPosition.x * 0.5f + 0.5f;
            vPosition.y = -vPosition.y * 0.5f + 0.5f;
            vPosition.z -= Bias; // 미세한 값 준거 zFighting 문제 때문에
        
            float factor = 0;
            float sum = 0;
            float avg = 0;
            const float dx = 1.0 / 2048.0f;
            depth = vPosition.z;
            const float2 offsets[9] =
            {
                float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		        float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
            };

	        [unroll]
            for (int i = 0; i < 9; ++i)
            {
                sum += ShadowMap.SampleCmpLevelZero(samShadow,
			    vPosition.xy + offsets[i], depth).r;
                ++avg;
            }
            factor = sum / avg;
            factor = factor < 0.3f ? 0.3f : factor;
            //output.tColor.rgb = output.tColor.rgb * factor;
            color.rgb = color.rgb * factor;
        }
    }

    [flatten]
    if (fogEnabled == true)
    {
        float fogFactor = saturate((distanceToEye - FogStart) / FogRange);

        color = lerp(color, FogColor, fogFactor);
    }

    color = IsHovering ? BrushHelper(color, input.Uv, BrushTexture) : color;
    
    output.tColor = color;
    output.pColor = float4(input.Uv.x, abs(1 - input.Uv.y), 0, 1);
    return output;
}

float4 PSGetShadow(DomainOutput input) : SV_TARGET
{
    float depth = input.dPosition.z / input.dPosition.w;
    
    return float4(depth, depth, depth, 1);
}


//-----------------------------------------------------------------------------
// States
//-----------------------------------------------------------------------------

RasterizerState FillMode
{
    FillMode = Wireframe;
};

DepthStencilState NoDoubleBlendDSS
{
    DepthEnable = true;
    StencilEnable = true;
    DepthWriteMask = All;
    DepthFunc = Less;
    StencilReadMask = 0xFF;
    StencilWriteMask = 0xFF;

    FrontFaceStencilFail = Keep;
    FrontFaceStencilDepthFail = Keep;
    FrontFaceStencilPass = Incr;
    FrontFaceStencilFunc = Less_Equal;

    BackFaceStencilFail = Keep;
    BackFaceStencilDepthFail = Keep;
    BackFaceStencilPass = Incr;
    BackFaceStencilFunc = Less_Equal;
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
        SetDomainShader(CompileShader(ds_5_0, DS(false)));
        SetPixelShader(CompileShader(ps_5_0, PS(true)));
    }

    pass P1
    {
        SetRasterizerState(FillMode);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetHullShader(CompileShader(hs_5_0, HS()));
        SetDomainShader(CompileShader(ds_5_0, DS(false)));
        SetPixelShader(CompileShader(ps_5_0, PS(true)));
    }

    pass P2
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetHullShader(CompileShader(hs_5_0, HS()));
        SetDomainShader(CompileShader(ds_5_0, DS(false)));
        SetPixelShader(CompileShader(ps_5_0, PSMR(true)));
    }

    pass P3
    {
        SetDepthStencilState(NoDoubleBlendDSS, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetHullShader(CompileShader(hs_5_0, HS()));
        SetDomainShader(CompileShader(ds_5_0, DS(false)));
        SetPixelShader(CompileShader(ps_5_0, PSMRBrush(false)));
    }

    pass P4
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetHullShader(CompileShader(hs_5_0, HS()));
        SetDomainShader(CompileShader(ds_5_0, DS(true)));
        SetPixelShader(CompileShader(ps_5_0, PSGetShadow()));
        //SetPixelShader(NULL);
    }
}