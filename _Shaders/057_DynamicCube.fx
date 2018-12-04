#include "000_Header.fx"

struct VertexOutput
{
    float4 Position : SV_POSITION0;
    float3 wPosition : POSITION1;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
};

VertexOutput VS(VertexTextureNormalTangent input)
{
    VertexOutput output;

    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;    

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);
    output.Uv = input.Uv;

    return output;
}

///////////////////////////////////////////////////////////////////////////////

VertexOutput VS_Model(VertexTextureNormalTangentBlend input)
{
    VertexOutput output;
    
    World = Bones[BoneIndex];
    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = WorldNormal(input.Normal);
    output.Tangent = normalize(mul(input.Tangent, (float3x3) World));
    output.Uv = input.Uv;

    return output;
}

///////////////////////////////////////////////////////////////////////////////

SamplerState Sampler
{
    AddressU = Wrap;
    AddressV = Wrap;
};

TextureCube CubeMap;

float4 PS(VertexOutput input) : SV_TARGET
{
    float3 normal = normalize(input.Normal);
    float3 toEye = normalize(ViewPosition - input.wPosition);

    float3 ambient = float3(0, 0, 0);
    float3 diffuse = DiffuseMap.Sample(TrilinearSampler, input.Uv);
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
    float4 factor = 0.0f;
    float4 diffuseColor = float4(ambient + diffuse + specular, 1);
    diffuseColor.a = Diffuse.a;
    

    //////////////////////////////////////////////////////
    
    // Reflect
    float3 incident = input.wPosition.xyz - ViewPosition;
    //float3 reflection = 2.0f * dot(-incident, normal) * normal + incident; //reflect(incident, normalize(normal));
    float3 reflection = reflect(incident, normalize(normal));
    float4 reflectionColor = CubeMap.Sample(TrilinearSampler, reflection); // refw 대신에 incident를 넣으면 투명이 됨.
    float4 color = reflectionColor;
    color = color * 0.8f + diffuseColor * 0.2f;
    
    //color = CubeMap.Sample(Sampler, float3(-normal.x, normal.y, normal.z));

    color.a = 1;

    return color;
}

///////////////////////////////////////////////////////////////////////////////

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        //SetRasterizerState(SolidCullNone);
        SetVertexShader(CompileShader(vs_5_0, VS_Model()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}