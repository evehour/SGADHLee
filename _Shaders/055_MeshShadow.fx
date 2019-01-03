#include "000_Header.fx"

struct VertexOutput
{
    float4 Position : SV_POSITION0;
    float3 wPosition : POSITION1;
    float4 lPosition : POSITION2;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
};

VertexOutput VS(VertexTextureNormalTangent input, uniform bool getDepth)
{
    VertexOutput output;

    output.wPosition = mul(input.Position, World).xyz;

    if(getDepth)
    {
        //output.Position = output.lPosition;
        output.Position = mul(float4(output.wPosition, 1), LightViewMatrix);
        output.Position = mul(output.Position, ShadowProjection);
    }
    else
    {
        output.Position = float4(output.wPosition, 1);
        output.Position = mul(output.Position, View);
        output.Position = mul(output.Position, Projection);
    }

    //output.lPosition = mul(input.Position, ShadowMatrix);
    output.lPosition = mul(float4(output.wPosition, 1), LightViewMatrix);
    output.lPosition = mul(output.lPosition, ShadowProjection);

    output.Uv = input.Uv;
    

    output.Normal = WorldNormal(input.Normal);

    return output;
}

///////////////////////////////////////////////////////////////////////////////

VertexOutput VS_Model(VertexTextureNormal input, uniform bool getDepth)
{
    VertexOutput output;

    World = Bones[BoneIndex];
    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;

    output.Position = mul(output.Position, getDepth ? LightViewMatrix : View);
    output.Position = mul(output.Position, getDepth ? ShadowProjection : Projection);
    
    output.lPosition = mul(input.Position, World);
    output.lPosition = mul(output.lPosition, LightViewMatrix);
    output.lPosition = mul(output.lPosition, ShadowProjection);

    output.Normal = WorldNormal(input.Normal);
    output.Uv = input.Uv;

    return output;
}

///////////////////////////////////////////////////////////////////////////////
Texture2D DepthMap;
SamplerComparisonState DepthCompSampler
{
    Filter = COMPARISON_MIN_MAG_MIP_LINEAR;

    ComparisonFunc = LESS_EQUAL;
    AddressU = Mirror;
    AddressV = Mirror;
    MaxAnisotropy = 1;
};
SamplerComparisonState samShadow
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
    BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    ComparisonFunc = LESS;
};

SamplerState Sampler
{
    AddressU = Wrap;
    AddressV = Wrap;
};

//---------------------------------------------------------------------------------------
// Performs shadowmap test to determine if a pixel is in shadow.
//---------------------------------------------------------------------------------------

static const float SMAP_SIZE = 2048.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcShadowFactor(SamplerComparisonState samShadow,
                       Texture2D shadowMap,
					   float4 shadowPosH)
{
	// Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;
	
	// Depth in NDC space.
    float depth = shadowPosH.z;

	// Texel size.
    const float dx = SMAP_DX;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };

	[unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(samShadow,
			shadowPosH.xy + (offsets[i] * ((depth < 0) ? -1 : 1)), depth).
        r;
    }

    return percentLit /= 9.0f;
}

float4 PS_Depth(VertexOutput input) : SV_TARGET
{
    float depth = input.lPosition.z / input.lPosition.w;
    float4 color = float4(depth, depth, depth, 1);

    return color;
}

float4 PS_Shadow(VertexOutput input) : SV_TARGET
{
    float NdotL;
    float3 normal = normalize(input.Normal);
    float3 toEye = normalize(ViewPosition - input.wPosition);
    float3 toLight = normalize(LightPosition - input.wPosition);
    NdotL = dot(normal, -toLight);

    float3 ambient = float3(0, 0, 0);
    float3 diffuse = float3(0, 0, 0);
    float3 specular = float3(0, 0, 0);
    float depth = 1;

    Material m = { Ambient, Diffuse, Specular, Shininess };
    DirectionalLight l = { LightAmbient, LightDiffuse, LightSpecular, LightDirection };

    float4 A, D, S;
    ComputeDirectionalLight(m, l, normal, toEye, A, D, S);
    ambient += A;
    diffuse += D;
    specular += S;

    [unroll]
    for (int i = 0; i < PointLightCount; i++)
    {
        ComputePointLight(m, PointLights[i], input.wPosition, normal, toEye, A, D, S);

        ambient += A;
        diffuse += D;
        specular += S;
    }
    
    [unroll]
    for (i = 0; i < SpotLightCount; i++)
    {
        ComputeSpotLight(m, SpotLights[i], input.wPosition, normal, toEye, A, D, S);

        ambient += A;
        diffuse += D;
        specular += S;
    }

    float4 color = float4(ambient + diffuse + specular, 1);
    color.a = Diffuse.a;

    //color = float4(1, 1, 1, 1);

    //float3 shadow = float3(1.0f, 1.0f, 1.0f);
    //shadow[0] = CalcShadowFactor(DepthCompSampler, DepthMap, input.lPosition);
    
    //return float4(color.xyz * shadow.xxx, 1);
    ////color = color * shadow[0];
    ////return color;

    float3 vPosition = input.lPosition.xyz / input.lPosition.w;

    if (vPosition.x < -1.0f || vPosition.x > 1.0f ||
        vPosition.y < -1.0f || vPosition.y > 1.0f ||
        vPosition.z < 0.0f || vPosition.z > 1.0f)
    {
        return color;
    }
    else
    {
        vPosition.x = vPosition.x * 0.5f + 0.5f;
        vPosition.y = -vPosition.y * 0.5f + 0.5f;
        vPosition.z -= 0.0001f; // 미세한 값 준거 zFighting 문제 때문에
        
        float factor = 0;
        float sum = 0;
        float avg = 0;
        for (float y = -1.5f; y <= 1.5f; y += 1.0f)
        {
            for (float x = -1.5f; x <= 1.5f; x += 1.0f)
            {
                float2 offset = float2(x / 2048.0f, y / 2048.0f);
            
                sum += DepthMap.SampleCmpLevelZero(DepthCompSampler, vPosition.xy, vPosition.z).r;
                ++avg;
            }
        }

        factor = sum / avg; // 4x4 라 16으로 나눈거

        //factor = sum / avg; // avg로 나누면 됨
        //depth = DepthMap.Sample(PointSampler, vPosition.xy).r;
        //depth = DepthMap.SampleCmpLevelZero(DepthCompSampler, vPosition.xy, vPosition.z);

        // 깊이가 작거나 같다는 얘기는 가까운 면이라는 얘기 -> 그 영역은 true
        //float factor = (float) vPosition.z <= depth;
        //float factor = saturate(depth - vPosition.z);

        color.rgb *= factor;
        return color;
    }
}

float4 PS(VertexOutput input) : SV_TARGET
{
    float3 normal = normalize(input.Normal);
    float3 toEye = normalize(ViewPosition - input.wPosition);

    float3 ambient = float3(0, 0, 0);
    float3 diffuse = float3(0, 0, 0);
    float3 specular = float3(0, 0, 0);


    Material m = { Ambient, Diffuse, Specular, Shininess };
    DirectionalLight l = { LightAmbient, LightDiffuse, LightSpecular, LightDirection };

    float4 A, D, S;
    ComputeDirectionalLight(m, l, normal, toEye, A, D, S);
    ambient += A;
    diffuse += D;
    specular += S;

    [unroll]
    for (int i = 0; i < PointLightCount; i++)
    {
        ComputePointLight(m, PointLights[i], input.wPosition, normal, toEye, A, D, S);

        ambient += A;
        diffuse += D;
        specular += S;
    }
    
    [unroll]
    for (i = 0; i < SpotLightCount; i++)
    {
        ComputeSpotLight(m, SpotLights[i], input.wPosition, normal, toEye, A, D, S);

        ambient += A;
        diffuse += D;
        specular += S;
    }

    float4 color = float4(ambient + diffuse + specular, 1);
    color.a = Diffuse.a;

    return color;
}

///////////////////////////////////////////////////////////////////////////////
RasterizerState Depth
{
    DepthBias = 10000;
    DepthBiasClamp = 0.0f;
    SlopeScaledDepthBias = 1.0f;
};

DepthStencilState NoDoubleBlendDSS
{
    DepthEnable = true;
    StencilEnable = true;
    DepthWriteMask = All;
    DepthFunc = Less;
    StencilReadMask = 0xFF;
    StencilWriteMask = 0xFF;

    FrontFaceStencilFail = Keep;
    FrontFaceStencilDepthFail = Keep;
    FrontFaceStencilPass = Incr;
    FrontFaceStencilFunc = Less_Equal;

    BackFaceStencilFail = Keep;
    BackFaceStencilDepthFail = Keep;
    BackFaceStencilPass = Incr;
    BackFaceStencilFunc = Less_Equal;
};

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS(false)));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Model(false)));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P2
    {
        SetRasterizerState(Depth);
        SetVertexShader(CompileShader(vs_5_0, VS(true)));
        //SetPixelShader(CompileShader(ps_5_0, PS_Depth()));
        SetPixelShader(NULL);
    }

    pass P3
    {
        SetDepthStencilState(NoDoubleBlendDSS, 0);
        SetVertexShader(CompileShader(vs_5_0, VS(false)));
        SetPixelShader(CompileShader(ps_5_0, PS_Shadow()));
    }
}