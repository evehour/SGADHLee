struct Data
{
    int Id;
    int Value;
    int Sum;
};

ConsumeStructuredBuffer<int> Input; // �Է¹���
AppendStructuredBuffer<Data> Output; // ��¹���

[numthreads(64, 1, 1)]
void CS(int3 id : SV_GroupThreadId)
{
    int data = Input.Consume();

    Data result;
    result.Id = id;
    result.Value = data;
    result.Sum = id + data;

    Output.Append(result);
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

