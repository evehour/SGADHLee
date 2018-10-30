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

    int UseBlend;
}

cbuffer VS_BoneIndex : register(b3)
{
    int BoneIndex;
}

cbuffer PS_Light : register(b0)
{
    float3 Direction;
    float PS_Light_Padding;

    float3 Position;
    float PS_Light_Padding2;
}

cbuffer PS_Material : register(b1)
{
    float4 Ambient;
    float4 Diffuse;
    float Shininess;
    float SpecularExp;
}

cbuffer PS_Lighting : register(b2)
{
    float4 LightPositionX;
    float4 LightPositionY;
    float4 LightPositionZ;
    
    float4 LightDirectionX;
    float4 LightDirectionY;
    float4 LightDirectionZ;

    float4 LightRange;
    
    float4 SpotOuter;
    float4 SpotInner;

    float4 CapsuleLength;
    
    float4 LightColorR;
    float4 LightColorG;
    float4 LightColorB;
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
    return normalize(mul(normal, (float3x3) World));
}

float3 WorldNormal(float3 normal, matrix world)
{
    return normalize(mul(normal, (float3x3) world));
}

float3 WorldTangent(float3 tangent, matrix world)
{
    return normalize(mul(tangent, (float3x3) world));
}

///////////////////////////////////////////////////////////////////////////////

struct Material
{
    float3 Normal;
    float4 DiffuseColor;
    float Shininess;
    float SpecularExp;
};

Material CreateMaterial(float3 normal, float2 uv)
{
    Material material;
    material.Normal = normalize(normal);
    
    //material.DiffuseColor = DiffuseMap.Sample(DiffuseSampler, uv);
    material.DiffuseColor = Diffuse;
    material.DiffuseColor.rgb *= material.DiffuseColor.rgb;

    // ºÒ¸° Æþ
    material.SpecularExp = SpecularExp;
    material.Shininess = Shininess;

    return material;
}

float4 dot4x4(float4 x1, float4 y1, float4 z1, float4 x2, float4 y2, float4 z2)
{
    return x1 * x2 + y1 * y2 + z1 * z2;
}

float4 dot4x1(float4 x, float4 y, float4 z, float3 b)
{
    return x * b.xxxx + y * b.yyyy + z * b.zzzz;
}

float3 Lighting(float3 wPosition, float3 cPosition, Material material)
{
    float3 eye = normalize(cPosition - wPosition);
    
    float4 capsuleStartX = wPosition.xxxx - LightPositionX;
    float4 capsuleStartY = wPosition.yyyy - LightPositionY;
    float4 capsuleStartZ = wPosition.zzzz - LightPositionZ;

    float4 distanceOnLine = dot4x4(capsuleStartX, capsuleStartY, capsuleStartZ, LightDirectionX, LightDirectionY, LightDirectionZ);
    float4 SafeCapsuleLength = max(CapsuleLength, 1e-6f);
    distanceOnLine = CapsuleLength *  saturate(distanceOnLine / SafeCapsuleLength);
    
    float4 pointOnLineX = LightPositionX + LightDirectionX * distanceOnLine;
    float4 pointOnLineY = LightPositionY + LightDirectionY * distanceOnLine;
    float4 pointOnLineZ = LightPositionZ + LightDirectionZ * distanceOnLine;
    float4 toLightX = pointOnLineX - wPosition.xxxx;
    float4 toLightY = pointOnLineY - wPosition.yyyy;
    float4 toLightZ = pointOnLineZ - wPosition.zzzz;
    float4 distanceToLightSqrt = dot4x4(toLightX, toLightY, toLightZ, toLightX, toLightY, toLightZ);
    float4 distanceToLight = sqrt(distanceToLightSqrt);

    //Phong Diffuse
    toLightX /= distanceToLight;
    toLightY /= distanceToLight;
    toLightZ /= distanceToLight;

    float4 NdotL = saturate(dot4x1(toLightX, toLightY, toLightZ, material.Normal));
    float3 color = float3(dot(LightColorR, NdotL), dot(LightColorG, NdotL), dot(LightColorB, NdotL));

    return color;
}