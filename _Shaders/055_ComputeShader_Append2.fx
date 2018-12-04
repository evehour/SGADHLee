struct Data
{
    int Id;
    int Value;
    int Sum;
};

//ConsumeStructuredBuffer<int> Input : register(u0); // 입력버퍼
//AppendStructuredBuffer<Data> Output : register(u1); // 출력버퍼
ConsumeStructuredBuffer<int> Input; // 입력버퍼
AppendStructuredBuffer<Data> Output; // 출력버퍼

[numthreads(16, 1, 1)]
void CS(int3 id : SV_GroupThreadId)
{
    int data;

    data = Input.Consume();
    GroupMemoryBarrierWithGroupSync();

    Data result;
    result.Id = id;
    result.Value = data;
    result.Sum = id + data;
    
    GroupMemoryBarrierWithGroupSync();
    Output.Append(result);
    GroupMemoryBarrierWithGroupSync();
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
        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}

