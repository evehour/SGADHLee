
//-----------------------------------------------------------------------------
// Compute Shader
//-----------------------------------------------------------------------------
static const int MaxBlurRadius = 8;
int BlurRadius = 5; // Blur Count

// ���� �ȼ�, ���� �ȼ�, �ڱ��ڽ�.
float Weights[MaxBlurRadius * 2 + 1];

static const int N = 256; // �� �ھ� ���� ����ϱ����� ���Ƿ� ����.
static const int CacheSize = (N + 2 * MaxBlurRadius); // �迭 �����ڿ��� ũ�⸦ ������ ����.
groupshared float4 Cache[CacheSize];
//�� �׷��, 32ų�ι���Ʈ�� �ִ�. �� �Ҵ��Ѵٸ� �׷��� �ڵ����� �����鼭 ó���ӵ��� ������.
//�㳪, �ǰ������ 24ų�ι���Ʈ��.

Texture2D Input;
// ������ �������� ������ ������ �� �ִ� ����� ����� ��¼���� �ִµ� �ȴٷ겨��.
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