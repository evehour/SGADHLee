#include "000_Header.fx"

struct VertexOutput_GizmoModel
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
};

struct VertexOutput_Line
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
};

struct VertexOutput_Quad
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
};

VertexOutput_GizmoModel VS_GizmoModel(VertexTextureNormal input)
{
    VertexOutput_GizmoModel output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = input.Normal;
    output.Uv = input.Uv;

    return output;
}

VertexOutput_Line VS_Line(VertexColor input)
{
    VertexOutput_Line output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Color = input.Color;

    return output;
}

///////////////////////////////////////////////////////////////////////////////

float4 DiffuseColor;

float4 PS_GizmoModel(VertexOutput_GizmoModel input) : SV_TARGET
{
    float4 color = 0;
    float4 diffuse = DiffuseColor;
    float3 normal = normalize(input.Normal);
    float NdotL = dot(-LightDirection, normal);
    
    color = diffuse * NdotL;

    return color;
}

float4 PS_Line(VertexOutput_Line input) : SV_TARGET
{
    return input.Color;
}

technique11 T0
{
    // GimoModel
    pass P0
    {
        SetRasterizerState(SolidCullNone);
        SetDepthStencilState(NoDepth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS_GizmoModel()));
        SetPixelShader(CompileShader(ps_5_0, PS_GizmoModel()));
    }

    // Line
    pass P1
    {
        SetRasterizerState(SolidCullNone);
        SetDepthStencilState(NoDepth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS_Line()));
        SetPixelShader(CompileShader(ps_5_0, PS_Line()));
    }

    // Line with Alphablend
    pass P2
    {
        SetRasterizerState(SolidCullNone);
        SetDepthStencilState(NoDepth, 0);
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS_GizmoModel()));
        SetPixelShader(CompileShader(ps_5_0, PS_GizmoModel()));
    }
}