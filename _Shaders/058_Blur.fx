
//-----------------------------------------------------------------------------
// Compute Shader
//-----------------------------------------------------------------------------
static const int MaxBlurRadius = 8;
int BlurRadius = 5; // Blur Count

// 양의 픽셀, 음의 픽셀, 자기자신.
float Weights[MaxBlurRadius * 2 + 1];

static const int N = 256; // 한 코어 개수 계산하기위해 임의로 넣음.
static const int CacheSize = (N + 2 * MaxBlurRadius); // 배열 공유자원의 크기를 지정할 거임.
groupshared float4 Cache[CacheSize];
//한 그룹당, 32킬로바이트가 최대. 더 할당한다면 그룹이 자동으로 열리면서 처리속도가 느려짐.
//허나, 권고사항은 24킬로바이트임.

Texture2D Input;
// 스레드 단위에서 순서를 제어할 수 있는 방법인 아토믹 어쩌구가 있는데 안다룰꺼임.
RWTexture2D<float4> Output;

[numthreads(N, 1, 1)]
void Horizontal(int3 groupThreadId : SV_GroupThreadID, int3 dispatchThreadId : SV_DispatchThreadID)
{    
    if (groupThreadId.x < BlurRadius)
    {
        int x = max(dispatchThreadId.x - BlurRadius, 0);
        Cache[groupThreadId.x] = Input[int2(x, dispatchThreadId.y)];
    }

    if (groupThreadId.x >= N - BlurRadius)
    {
        int x = min(dispatchThreadId.x + BlurRadius, Input.Length.x - 1);

        Cache[groupThreadId.x + 2 * BlurRadius] = Input[int2(x, dispatchThreadId.y)];
    }

    Cache[groupThreadId.x + BlurRadius] = Input[min(dispatchThreadId.xy, Input.Length.xy - 1)];
    GroupMemoryBarrierWithGroupSync();

    float4 color = 0;
    //[unroll]
    for (int i = -BlurRadius; i <= BlurRadius; ++i)
    {
        int k = groupThreadId.x + BlurRadius + i;
        color += Weights[i + BlurRadius] * Cache[k];
    }

    Output[dispatchThreadId.xy] = color;
}



[numthreads(1, N, 1)]
void Vertical(int3 groupThreadId : SV_GroupThreadId, int3 dispatchThreadId : SV_DispatchThreadId)
{
    if (groupThreadId.y < BlurRadius)
    {
        int y = max(dispatchThreadId.y - BlurRadius, 0);
        Cache[groupThreadId.y] = Input[int2(dispatchThreadId.x, y)];

    }

    if (groupThreadId.y >= N - BlurRadius)
    {
        int y = min(dispatchThreadId.y + BlurRadius, Input.Length.y - 1);

        Cache[groupThreadId.y + 2 * BlurRadius] = Input[int2(dispatchThreadId.x, y)];
    }

    Cache[groupThreadId.y + BlurRadius] = Input[min(dispatchThreadId.xy, Input.Length.xy - 1)];
    GroupMemoryBarrierWithGroupSync();

    float4 color = 0;
    //[unroll]
    for (int i = -BlurRadius; i <= BlurRadius; i++)
    {
        int k = groupThreadId.y + BlurRadius + i;
        color += Weights[i + BlurRadius] * Cache[k];
    }

    Output[dispatchThreadId.xy] = color;
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
        SetComputeShader(CompileShader(cs_5_0, Horizontal()));
    }

    pass P1
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, Vertical()));
    }
}