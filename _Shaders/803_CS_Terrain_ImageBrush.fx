//-----------------------------------------------------------------------------
// Compute Shader
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Terrain Brush
//-----------------------------------------------------------------------------
float PI = 3.14159265f;

float2 HeightMapPixSize = float2(2049, 2049);
float MaxHeight = 50.0f;
static const int N = 16; // 한 코어 개수 계산하기위해 임의로 넣음.
float BrushPower;

Texture2D OriginMap;
Texture2D ProjBrushMap;
RWTexture2D<float> Output;

[numthreads(N,N,1)]
void BrushNormal(int3 groupThreadId : SV_GroupThreadID, int3 dispatchThreadId : SV_DispatchThreadID)
{
    int2 xy;
    int2 LenghtXY;
    OriginMap.GetDimensions(LenghtXY.x, LenghtXY.y);

    xy.x = min(dispatchThreadId.x, (LenghtXY.x - 1));
    xy.y = min(dispatchThreadId.y, (LenghtXY.y - 1));
    
    float4 projColor = ProjBrushMap[xy];
    float4 hightColor = OriginMap[xy];

    float val = 1.0f;
    hightColor.r += (projColor.g > 0) ? BrushPower : 0;

    Output[dispatchThreadId.xy] = hightColor;
}

float2 PickPixelPos = float2(1024, 1024);
float2 BrushRate;
[numthreads(N, N, 1)]
void BrushFlat(int3 groupThreadId : SV_GroupThreadID, int3 dispatchThreadId : SV_DispatchThreadID)
{
    int2 xy;
    int2 LenghtXY;
    OriginMap.GetDimensions(LenghtXY.x, LenghtXY.y);

    xy.x = min(dispatchThreadId.x, (LenghtXY.x - 1));
    xy.y = min(dispatchThreadId.y, (LenghtXY.y - 1));
    
    float4 projColor = ProjBrushMap[xy];
    float4 hightColor = OriginMap[xy];

    float val = 1.0f;
    float incValue = 0.0f;

    if (projColor.g > 0)
    {
        xy.y = (LenghtXY.y - 1) - min(dispatchThreadId.y, (LenghtXY.y - 1));
        
        float d = distance(PickPixelPos, xy);
        float md = distance(PickPixelPos, PickPixelPos + (BrushRate * 0.75));
        //incValue = (1.0f - cos(1.0f - (d / md))) * 5.0f * BrushPower;
        incValue = abs((cos(PI * d / md) + 0.5f));

    }
    hightColor.r += (incValue * BrushPower);

    Output[dispatchThreadId.xy] = hightColor;
}


[numthreads(N, N, 1)]
void BrushSmooth(int3 groupThreadId : SV_GroupThreadID, int3 dispatchThreadId : SV_DispatchThreadID)
{
    int2 xy;
    int2 xy2;
    int2 LenghtXY;
    OriginMap.GetDimensions(LenghtXY.x, LenghtXY.y);

    xy.x = min(dispatchThreadId.x, (LenghtXY.x - 1));
    xy.y = min(dispatchThreadId.y, (LenghtXY.y - 1));
    
    float4 projColor = ProjBrushMap[xy];
    float4 hightColor = OriginMap[xy];

    float val = 1.0f;
    float incValue = 0.0f;

    if (projColor.g > 0)
    {
        float sum = 0;
        int num = 0;
        for (int y1 = (xy.y - 1); y1 < (xy.y + 2); ++y1)
        {
            for (int x1 = (xy.x - 1); x1 < (xy.x + 2); ++x1)
            {
                xy2.x = min(x1, (LenghtXY.x - 1));
                xy2.y = min(y1, (LenghtXY.y - 1));
                xy2.x = max(xy2.x, 0);
                xy2.y = max(xy2.y, 0);

                sum += OriginMap[xy2].r;
                ++num;
            }
        }

        hightColor.r = sum / ((float) num);
    }

    Output[dispatchThreadId.xy] = hightColor;
}

//-----------------------------------------------------------------------------
// Terrain Save
//-----------------------------------------------------------------------------

RWStructuredBuffer<float> SaveData;
[numthreads(N, N, 1)]
void SaveHeightMap(int3 groupThreadId : SV_GroupThreadID, int3 dispatchThreadId : SV_DispatchThreadID)
{
    int k = 0;
    int2 xy;
    int2 LenghtXY;
    OriginMap.GetDimensions(LenghtXY.x, LenghtXY.y);
    
    xy.x = min(dispatchThreadId.x, (LenghtXY.x - 1));
    xy.y = min(dispatchThreadId.y, (LenghtXY.y - 1));
    
    float4 hightColor = OriginMap[xy];
    SaveData[xy.x + xy.y * LenghtXY.x] = hightColor.r / MaxHeight * 255.0f;
}

//-----------------------------------------------------------------------------
// Terrain Command
//-----------------------------------------------------------------------------

[numthreads(N, N, 1)]
void CopyHeightMap(int3 groupThreadId : SV_GroupThreadID, int3 dispatchThreadId : SV_DispatchThreadID)
{
    int2 xy;
    int2 LenghtXY;
    OriginMap.GetDimensions(LenghtXY.x, LenghtXY.y);

    xy.x = min(dispatchThreadId.x, (LenghtXY.x - 1));
    xy.y = min(dispatchThreadId.y, (LenghtXY.y - 1));

    Output[dispatchThreadId.xy] = OriginMap[xy];
}

//-----------------------------------------------------------------------------
// Technique
//-----------------------------------------------------------------------------

technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, BrushNormal()));
    }

    pass P1
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, BrushFlat()));
    }

    pass P2
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, BrushSmooth()));
    }

    pass P3
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, SaveHeightMap()));
    }

    pass P4
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, CopyHeightMap()));
    }
}
