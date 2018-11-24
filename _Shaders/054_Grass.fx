#include "000_Header.fx"

float2 WindDirection = float2(1.0f, 0.0f);
float DeltaSTime = 0.0f;
float ShakeFactor = 1.0f;

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------
struct VertexInput
{
    float4 Position : POSITION0;

    matrix World : INSTANCE0;
    uint Type : INSTANCE4;
    uint ShakeType : INSTANCE5;
    float WindLimitFactor : INSTANCE6;

    uint InstanceID : SV_INSTANCEID0;
};

struct VertexOutput
{
    float4 Center : POSITION0;

    matrix World : WORLD0;
    uint Type : TYPE0;
    uint ShakeType : TYPE1;
    float WindLimitFactor : WINDFACTOR0;

    uint id : INSTANCEID0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Center = input.Position;

    output.World = input.World;
    output.Type = input.Type;
    output.ShakeType = input.ShakeType;
    output.WindLimitFactor = input.WindLimitFactor;

    output.id = input.InstanceID;

    return output;
}



//-----------------------------------------------------------------------------
// Geometry Shader
//-----------------------------------------------------------------------------
struct GeometryOutput
{
    float4 Position : SV_POSITION0;
    float3 wPosition : POSITION1;
    float2 Uv : UV0;
    uint Type : TYPE0;
};

static const float2 TexCoord[4] =
{
    float2(0.0f, 1.0f), // downleft
    float2(0.0f, 0.0f), // upleft
    float2(1.0f, 1.0f), // downright
    float2(1.0f, 0.0f), // upright
};

[maxvertexcount(4)]
void GS(
    point VertexOutput input[1],
    inout TriangleStream<GeometryOutput> stream)
{
    float3 up = float3(0, 1, 0);
    //float4 wPos = input[0].Center;//mul(input[0].Center, input[0].World);
    float4 wPos = mul(input[0].Center, input[0].World);
    float3 look = ViewPosition - wPos.xyz;
    look.y = 0.0f; // 수직고정
    look = normalize(look);

    float3 right = cross(up, look);
    
    float halfWidth = 0.5f;
    float halfHeight = 0.5f;
    
    float3 shaking;
    float4 v[4];

    
    float limit = sin(DeltaSTime * ShakeFactor * (((wPos.x * -WindDirection.x * 0.0039215f * 0.5f) + 1 - (wPos.z * WindDirection.y * 0.0039215f)) * 0.5f * 0.5f));
    limit = lerp(-input[0].WindLimitFactor, input[0].WindLimitFactor, (limit + 1.0f) * 0.5f);

    [branch]
    if(input[0].ShakeType == 0)
    {
        shaking = float3(limit, 0, 0);
    }
    else if (input[0].ShakeType == 1)
    {
        shaking = float3(0, 0, limit);
    }

    v[0] = float4(input[0].Center.xyz + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(input[0].Center.xyz + shaking + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(input[0].Center.xyz - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(input[0].Center.xyz + shaking - halfWidth * right + halfHeight * up, 1.0f);

    GeometryOutput output;
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        output.Position = mul(v[i], input[0].World);
        output.Position = mul(output.Position, View);
        output.Position = mul(output.Position, Projection);
        output.wPosition = v[i].xyz;
        
        output.Uv = TexCoord[i];

        output.Type = input[0].Type;

        stream.Append(output);
    }

}


//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------
Texture2DArray Maps;

float4 PS(GeometryOutput input) : SV_TARGET
{
    float4 color = Maps.Sample(TrilinearSampler, float3(input.Uv, input.Type));
    float NdotL = dot(-LightDirection, normalize(float3(0, 1, 0)));
    color.xyz = color.xyz * NdotL;
    return color;
}

technique11 T0
{
    pass P0
    {
        SetRasterizerState(SolidCullNone);
        SetBlendState(AlphaBlend1, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(CompileShader(gs_5_0, GS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}
