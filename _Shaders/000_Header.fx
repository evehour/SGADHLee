
//-----------------------------------------------------------------------------
// CBuffer
//-----------------------------------------------------------------------------
cbuffer CB_VP
{
    matrix View;
    matrix Projection;
};

cbuffer CB_World
{
    matrix World;
};

float4 Color = float4(1, 0, 0, 1);
