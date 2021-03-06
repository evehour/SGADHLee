//struct Data
//{
//    int Id;
//    int Value;
//    int Sum;
//};

////ConsumeStructuredBuffer<int> Input : register(u0); // 입력버퍼
////AppendStructuredBuffer<Data> Output : register(u1); // 출력버퍼
//ConsumeStructuredBuffer<int> Input : register(u0); // 입력버퍼
//AppendStructuredBuffer<Data> Output : register(u1); // 출력버퍼

//[numthreads(64, 1, 1)]
//void CS(int3 id : SV_GroupThreadId)
//{
//    int data = Input.Consume();

//    Data result;
//    result.Id = id;
//    result.Value = data;
//    result.Sum = id + data;

//    Output.Append(result);
//}

////-----------------------------------------------------------------------------
//// Technique
////-----------------------------------------------------------------------------

//technique11 T0
//{
//    pass P0
//    {
//        SetVertexShader(NULL);
//        SetPixelShader(NULL);
//        SetComputeShader(CompileShader(cs_5_0, CS()));
//    }
//}




///////////////////////////////////////////////////////////////////////////////
struct Particle
{
    float3 Position;
    float3 Velocity;
    float3 Accelation;
};

float TimeStep = 1.0f / 60.0f;

ConsumeStructuredBuffer<Particle> cInput : register(u0); // 입력버퍼
AppendStructuredBuffer<Particle> aOutput : register(u1); // 출력버퍼

[numthreads(1, 1, 1)]
void CS()
{
    // 아예 독립적이기 때문에 아이디도 필요없다.
    Particle p = cInput.Consume();
    
    p.Velocity += p.Accelation * TimeStep;
    p.Position += p.Velocity * TimeStep;

    aOutput.Append(p);
}
