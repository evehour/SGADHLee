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

float n1 = 1.0f;
float n2 = 1.02f;
// 스넬의 법칙
// : n1 * sin(Theta) = n2 * sin(Theta)

float3 GetNormal(float3 normal, float2 uv, Texture2D map, SamplerState state, uniform int power = 1)
{
    float3 B = { 1, 0, 0 };
    float3 T = { 0, 1, 0 };
    float3 N = { 0, 0, 1 };

    N = normalize(normal);
    T = normalize(cross(N, B));
    B = normalize(cross(T, N));

    float3 C1 = 0.0f;
    float3 C = 2.0f * map.Sample(state, uv * 2.5f).xyz - 1.0f;
    C = normalize(C);

   
    C1.x = B.x * C.x + T.x * C.y + N.x * C.z;
    C1.y = B.y * C.x + T.y * C.y + N.y * C.z;
    C1.z = B.z * C.x + T.z * C.y + N.z * C.z;
    C = normalize(C1);

    return C;
}

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
    
    normal = GetNormal(input.Normal, input.Uv, NormalMap, TrilinearSampler);
    // Reflect
    float3 incident = input.wPosition.xyz - ViewPosition;
    //float3 reflection = 2.0f * dot(-incident, normal) * normal + incident; //reflect(incident, normalize(normal));
    float3 reflection = reflect(incident, normalize(normal));
    float4 reflectionColor = CubeMap.Sample(TrilinearSampler, reflection); // refw 대신에 incident를 넣으면 투명이 됨.
    
    // Refraction
    float k = n2 / n1;
    float3 refraction = refract(incident, normal, 0.5f);
    float4 refractionColor = CubeMap.Sample(TrilinearSampler, refraction);
    
    float w = dot(reflection, normal);
    w *= w;
    w = smoothstep(0.4f, 0.7f, w);
    w = 1.0f - w * 0.8f;
    
    float4 color = lerp(reflectionColor, refractionColor, w);
    color = color * 0.8f + diffuseColor * 0.2f;
    
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
        SetVertexShader(CompileShader(vs_5_0, VS_Model()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}