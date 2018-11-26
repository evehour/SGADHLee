#include "000_Header.fx"


//-----------------------------------------------------------------------------
// Compute Shader
//-----------------------------------------------------------------------------
#define MAX_BUFFER_SIZE 512
#define TRANSPOSE_BLOCK_SIZE 16

cbuffer CB_CSCBuffer
{
    uint g_iLevel;
    uint g_iLevelMask;
    uint g_iWidth;
    uint g_iHeight;
};

uint MaximumIndex;

StructuredBuffer<uint> Inputs;
RWStructuredBuffer<uint> Datas;
groupshared uint shared_data[MAX_BUFFER_SIZE];

[numthreads(MAX_BUFFER_SIZE, 1, 1)]
void BitonicSort(uint3 Gid : SV_GroupID,
                  uint3 DTid : SV_DispatchThreadID,
                  uint3 GTid : SV_GroupThreadID,
                  uint GI : SV_GroupIndex,
                  uniform bool postorder)
{
    // Load shared data
    shared_data[GI] = Datas[DTid.x];
    GroupMemoryBarrierWithGroupSync();
    
    // Sort the shared data
    for (uint j = g_iLevel >> 1; j > 0; j >>= 1)
    {
        bool helper = postorder ? (shared_data[GI & ~j] > shared_data[GI | j]) : (shared_data[GI & ~j] <= shared_data[GI | j]);
        uint idx = (helper == (bool) (g_iLevelMask & DTid.x)) ? (GI ^ j) : GI;
        
        uint result = shared_data[idx];

        GroupMemoryBarrierWithGroupSync();
        shared_data[GI] = result;
        GroupMemoryBarrierWithGroupSync();
    }
    
    // Store shared data
    Datas[DTid.x] = shared_data[GI];
}
///////////////////////////////////////////////////////////////////////////////
// Matrix Transpose Compute Shader
groupshared unsigned int transpose_shared_data[TRANSPOSE_BLOCK_SIZE * TRANSPOSE_BLOCK_SIZE];

[numthreads(TRANSPOSE_BLOCK_SIZE, TRANSPOSE_BLOCK_SIZE, 1)]
void MatrixTranspose(uint3 Gid : SV_GroupID,
                      uint3 DTid : SV_DispatchThreadID,
                      uint3 GTid : SV_GroupThreadID,
                      uint GI : SV_GroupIndex)
{
    transpose_shared_data[GI] = Inputs[DTid.y * g_iWidth + DTid.x];
    GroupMemoryBarrierWithGroupSync();
    uint2 XY = DTid.yx - GTid.yx + GTid.xy;
    Datas[XY.y * g_iHeight + XY.x] = transpose_shared_data[GTid.x * TRANSPOSE_BLOCK_SIZE + GTid.y];
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
        SetComputeShader(CompileShader(cs_5_0, BitonicSort(true)));
    }

    pass P2
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, MatrixTranspose()));
    }
}

technique11 T1
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, BitonicSort(false)));
    }

    pass P2
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, MatrixTranspose()));
    }
}

