struct Particle
{
    float3 Position;
    float3 Velocity;
    float3 Accelation;
};

float TimeStep = 1.0f / 60.0f;

ConsumeStructuredBuffer<Particle> cInput; // �Է¹���
AppendStructuredBuffer<Particle> aOutput; // ��¹���

[numthreads(16, 1, 1)]
void CS_Particle()
{
    // �ƿ� �������̱� ������ ���̵� �ʿ����.
    Particle p = cInput.Consume();
    
    p.Velocity += p.Accelation * TimeStep;
    p.Position += p.Velocity * TimeStep;

    aOutput.Append(p);
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
        SetComputeShader(CompileShader(cs_5_0, CS_Particle()));
    }
}

