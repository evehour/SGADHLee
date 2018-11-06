cbuffer VS_ViewProjection : register(b0) // register buffer 0번 쓰겠다는거
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

cbuffer PS_Light : register(b0) // 버텍스 쉐이더랑 별개이므로 다시 0번부터
{
    float3 Direction;
    float PS_Light_Padding;
    
    float3 Position;
    float PS_Light_Padding2;
}

// material 재질 의미
cbuffer PS_Material : register(b1)
{
    float4 Ambient;
    float4 Diffuse; // 알파값이 없어서 Diffuse alpha의 Shininess 쓰기도함
    float4 Specular;
    float Shininess; // specular power라고 쓰기도 함
}

struct LightingData
{
   // 세로열이 조명 하나
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
};

cbuffer PS_Lighting : register(b2)
{
    LightingData LightingDatas[6];

    int LightingCount;
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

struct VertexColorTextureNormal
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
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

///////////////////////////////////////////////////////////////////////////////

matrix BoneWorld()
{
    //return mul(Bones[BoneIndex], BoneScale);
    return Bones[BoneIndex];
}

matrix SkinWorld(float4 blendIndices, float4 blendWeights)
{
    float4x4 transform = 0;
    transform += mul(blendWeights.x, Bones[(uint) blendIndices.x]);
    transform += mul(blendWeights.y, Bones[(uint) blendIndices.y]);
    transform += mul(blendWeights.z, Bones[(uint) blendIndices.z]);
    transform += mul(blendWeights.w, Bones[(uint) blendIndices.w]);
    
   //transform = mul(transform, BoneScale);
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

struct Material
{
    float3 Normal;
    float4 DiffuseColor;
    float4 SpecularColor; // 이것의 a를 강도로 씀
    float Shininess;
};

Material CreateMaterial(float3 normal, float2 uv)
{
    Material material;
    material.Normal = normalize(normal);
    //material.DiffuseColor = DiffuseMap.Sample(DiffuseSampler, uv);
    material.DiffuseColor = Diffuse;
    material.DiffuseColor.rgb *= material.DiffuseColor.rgb; // 선형색 공간으로 바꾸는거

   // 블링 퐁 모델 
    material.SpecularColor = Specular;
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

float3 Lighting(LightingData data, float3 wPosition, float3 cPosition, Material material)
{
    float3 eye = normalize(cPosition - wPosition);

    float4 capsuleStartX = wPosition.xxxx - data.LightPositionX;
    float4 capsuleStartY = wPosition.yyyy - data.LightPositionY;
    float4 capsuleStartZ = wPosition.zzzz - data.LightPositionZ;

    float4 distanceOnLine = dot4x4(
   capsuleStartX, capsuleStartY, capsuleStartZ,
   data.LightDirectionX, data.LightDirectionY, data.LightDirectionZ);
    float4 SafeCapsuleLength = max(data.CapsuleLength, 1e-6f); // 분모 0으로 안하게 하기 위해서 10-6, 0.000001
    distanceOnLine = saturate(distanceOnLine / SafeCapsuleLength) * data.CapsuleLength;

    float4 pointOnLineX = data.LightPositionX + data.LightDirectionX * distanceOnLine;
    float4 pointOnLineY = data.LightPositionY + data.LightDirectionY * distanceOnLine;
    float4 pointOnLineZ = data.LightPositionZ + data.LightDirectionZ * distanceOnLine;
    float4 toLightX = pointOnLineX - wPosition.xxxx;
    float4 toLightY = pointOnLineY - wPosition.yyyy;
    float4 toLightZ = pointOnLineZ - wPosition.zzzz;
    float4 distanceToLightSqrt = dot4x4(toLightX, toLightY, toLightZ, toLightX, toLightY, toLightZ);
    float4 distanceToLight = sqrt(distanceToLightSqrt);

   // Phong Diffuse
    toLightY /= distanceToLight;
    toLightZ /= distanceToLight;
    toLightX /= distanceToLight;

    float4 NdotL = saturate(dot4x1(toLightX, toLightY, toLightZ, material.Normal));
    //float3 color = float3(
    //   dot(data.LightColorR, NdotL),
    //   dot(data.LightColorG, NdotL),
    //   dot(data.LightColorB, NdotL)
    //);

    // Blinn Specular
    eye = normalize(eye);
    float4 halfWayX = eye.xxxx + toLightX;
    float4 halfWayY = eye.yyyy + toLightY;
    float4 halfWayZ = eye.zzzz + toLightZ;

    // 2개 더한 벡터
    float4 halfWaySize = sqrt(
        dot4x4(
            halfWayX, halfWayY, halfWayZ, halfWayX, halfWayY, halfWayZ
        )
    );
    float4 NdotH = saturate(dot4x1(halfWayX / halfWaySize, halfWayY / halfWaySize, halfWayZ / halfWaySize, material.Normal));
    float4 specular = pow(NdotH, material.Shininess.xxxx) * material.SpecularColor.a;
    //color += float3(
    //   dot(data.LightColorR, specular),
    //   dot(data.LightColorG, specular),
    //   dot(data.LightColorB, specular)
    //);

    // Cone attenuation
    float4 cosAngle = dot4x4(
        data.LightDirectionX, data.LightDirectionY, data.LightDirectionZ,
        toLightX, toLightY, toLightZ
    );
    float4 conAttenuation = saturate((cosAngle - data.SpotOuter) * data.SpotInner);
    conAttenuation *= conAttenuation;

    // Attenuation
    float4 distanceToLightNormal = 1.0f - saturate(distanceToLight * data.LightRange);
    float4 attenuation = distanceToLightNormal * distanceToLightNormal;
    attenuation *= conAttenuation;

    // 최종 색
    float4 pixelIntensity = (NdotL + specular) * attenuation;
    float3 color = float3(
       dot(data.LightColorR, pixelIntensity),
       dot(data.LightColorG, pixelIntensity),
       dot(data.LightColorB, pixelIntensity)
    );
    color *= material.DiffuseColor;

    return color;
    //return specular.rgb;

}