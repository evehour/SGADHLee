//-----------------------------------------------------------------------------
// Compute Shader
//-----------------------------------------------------------------------------
float2 mPosition;
Texture2D InputTex;
RWStructuredBuffer<float4> Output;

static const int N = 512; // 한 코어 개수 계산하기위해 임의로 넣음.

[numthreads(N,1,1)]
void MousePicking(int3 groupThreadId : SV_GroupThreadID, int3 dispatchThreadId : SV_DispatchThreadID)
{
    int2 xy;
    int2 LenghtXY;
    InputTex.GetDimensions(LenghtXY.x, LenghtXY.y);

    xy.x = min(dispatchThreadId.x, (LenghtXY.x - 1));
    xy.y = min(dispatchThreadId.y, (LenghtXY.y - 1));
    
    float4 projColor = InputTex[xy];
    
    if (xy.x == (int) mPosition.x && xy.y == (int)mPosition.y)
    {
        Output[0] = projColor;
    }
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
        SetComputeShader(CompileShader(cs_5_0, MousePicking()));
    }
}
