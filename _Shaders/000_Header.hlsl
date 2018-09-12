cbuffer VS_ViewProjection : register(b0)
{
    matrix View;
    matrix Projection;
    matrix ViewInverse;
}

cbuffer VS_World : register(b1)
{
    matrix World;
}

cbuffer VS_Bones : register(b2)
{
    matrix Bones[128];
}

cbuffer VS_BoneIndex : register(b3)
{
    int BoneIndex;
}

cbuffer PS_Light : register(b0)
{
    float3 Direction;
}

cbuffer PS_Material : register(b1)
{
    float4 Diffuse;
    float4 Specular;
    float Shininess;
}

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSampler : register(s0);

Texture2D SpecularMap : register(t1);
SamplerState SpecularSampler : register(s1);

Texture2D NormalMap : register(t2);
SamplerState NormalSampler : register(s2);

Texture2D DetailMap : register(t3);
SamplerState DetailSampler : register(s3);

struct VertexColor
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
};

struct VertexColorNormal
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
    float3 Normal : NORMAL0;
};

struct VertexTexture
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
};

struct VertexTextureNormal
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
};

struct VertexTextureNormalTangent
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
};

struct VertexTextureNormalTangentBlend
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
    float4 BlendIndices : BLENDINDICES0;
    float4 BlendWeights : BLENDWEIGHTS0;
};

struct VertexColorTextureNormal
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
};

///////////////////////////////////////////////////////////////////////////////

matrix BoneWorld()
{
    return Bones[BoneIndex];
}

matrix SkinWorld(float4 blendIndices, float4 blendWeights)
{
    float4x4 transform = 0;
    transform += mul(blendWeights.x, Bones[(uint) blendIndices.x]);
    transform += mul(blendWeights.y, Bones[(uint) blendIndices.y]);
    transform += mul(blendWeights.z, Bones[(uint) blendIndices.z]);
    transform += mul(blendWeights.w, Bones[(uint) blendIndices.w]);

    return transform;
}

float3 CameraPosition()
{
    return ViewInverse._41_42_43;
}

float3 WorldViewDirection(float4 wPosition)
{
    return normalize(CameraPosition() - wPosition.xyz);
}

float3 WorldNormal(float3 normal)
{
    normal = normalize(mul(normal, (float3x3) World));

    return normal;
}

float3 WorldNormal(float3 normal, matrix world)
{
    normal = normalize(mul(normal, (float3x3) world));

    return normal;
}

float3 WorldTangent(float3 tangent, matrix world)
{
    tangent = normalize(mul(tangent, (float3x3) world));

    return tangent;
}

///////////////////////////////////////////////////////////////////////////////


void DiffuseLighting(inout float4 color, float3 normal)
{
    float intensity = saturate(dot(normal, -Direction));

    color = color + Diffuse * intensity;
}

void DiffuseLighting(inout float4 color, float4 diffuse, float3 normal)
{
    float intensity = saturate(dot(normal, -Direction));

    color = color + Diffuse * diffuse * intensity;
}

void SpecularLighting(inout float4 color, float3 normal, float3 viewDirection)
{
    float3 reflection = reflect(Direction, normal);
    float intensity = saturate(dot(reflection, viewDirection));
    float specular = pow(intensity, Shininess);

    color = color + Specular * specular;
}

void SpecularLighting(inout float4 color, float4 specularMap, float3 normal, float3 viewDirection)
{
    float3 reflection = reflect(Direction, normal);
    float intensity = saturate(dot(reflection, viewDirection));
    float specular = pow(intensity, Shininess);

    color = color + Specular * specular * specularMap;
}

void NormalMapping(inout float4 color, float4 normalMap, float3 normal, float3 tangent)
{
    float3 N = normal; //Z축
    float3 T = normalize(tangent - dot(tangent, N) * N); // X축 그람-슈미츠
    float3 B = cross(T, N); //Y축

    float3x3 TBN = float3x3(T, B, N);

    float3 coord = 2.0f * normalMap - 1.0f;
    float3 bump = mul(coord, TBN); // bumpMapping == normalMapping 용어가 같음.

    float intensity = saturate(dot(bump, -Direction)); // 음영을 만드는 식

    color = color * intensity;
}

void DetailMapping(inout float4 color, float4 detailMap, float bright)
{
    color = color * detailMap * bright;
}

///////////////////////////////////////////////////////////////////////////////

struct PointLight
{
    float3 Position;
    float Range;
    float3 Color;
    float Intensity;
};

cbuffer PS_PointLights : register(b2)
{
    PointLight PointLights[32];

    int PointLightCount;
}

void PointLighting(inout float4 color, PointLight light, float4 wPosition, float3 normal)
{
    float dist = distance(light.Position, wPosition.xyz);
    float intensity = saturate((light.Range - dist) / light.Range);

    intensity = pow(intensity, light.Intensity); // 면적의 강도

    color = light.Intensity > 0 ? (color + float4(light.Color, 0) * intensity) : 0;
}

///////////////////////////////////////////////////////////////////////////////

struct SpotLight
{
    float3 Position;
    float InnerAngle;
    float3 Color;
    float OuterAngle;
    float3 Direction;

    float SpotLight_Padding;
};

cbuffer PS_SpotLights : register(b3)
{
    SpotLight SpotLights[32];

    int SpotLightCount;
}

void SpotLighting(inout float4 color, SpotLight light, float4 wPosition, float3 normal)
{
    float3 lightDir = normalize(light.Position - wPosition.xyz);

    float intensity = 0;
    float lightAngle = dot(-light.Direction, lightDir);
    
    intensity = (lightAngle > 0) ? (smoothstep(light.InnerAngle, light.OuterAngle, lightAngle)) : 0;

    color = color + float4(light.Color, 0) * intensity;
}

///////////////////////////////////////////////////////////////////////////////

struct AreaLight
{
    float3 Position;
    float AreaLightWidth;
    float3 Direction;
    float AreaLightHeight;
    float3 Color;
    
    float Intensity;
};

cbuffer PS_AreaLights : register(b4)
{
    AreaLight AreaLights[32];
    int AreaLightCount;
}

float AreaCheck(AreaLight light, float4 wPosition)
{
    int b = 0;
    float3 left, right, front, back;
    left = right = front = back = light.Position;
    left.x -= light.AreaLightWidth / 2.0f;
    right.x += light.AreaLightWidth / 2.0f;
    front.z += light.AreaLightHeight / 2.0f;
    back.z -= light.AreaLightHeight / 2.0f;

    b += (wPosition.x >= left.x) ? 1 : 0;
    b += (wPosition.x <= right.x) ? 1 : 0;
    b += (wPosition.z >= back.z) ? 1 : 0;
    b += (wPosition.z <= front.z) ? 1 : 0;
    b = (b > 3) ? 1 : -1;
    return b;
}

void AreaLighting(inout float4 color, AreaLight light, float4 wPosition, float3 normal)
{
    float3 lightDir = normalize(light.Position - wPosition.xyz);
    float intensity = 0;
    float lightAngle = dot(-light.Direction, lightDir);

    float dist = distance(light.Position, wPosition.xyz);
    float intensity2 = saturate(((light.Intensity + dist) - dist) / (light.Intensity + dist));
    
    intensity = (lightAngle > 0) ? (smoothstep(0, 2, lightAngle) * intensity2) : 0;
    intensity = (AreaCheck(light, wPosition) > 0) ?
                 intensity :
                 //((intensity == 0) ?
                 //        0 :
                 //        lerp(intensity, 0, lightAngle * 2));
                 0;

    color = color + float4(light.Color, 0) * intensity;
    //color = float4(intensity, intensity, intensity, intensity);
}