
//-----------------------------------------------------------------------------
// CBuffer
//-----------------------------------------------------------------------------
cbuffer CB_PerFrame
{
    row_major matrix View;

    float3 ViewDirection;
    float Time;
    float3 ViewPosition;
};

float3 WorldViewDirection(float4 wPosition)
{
    return normalize(ViewPosition - wPosition.xyz);
}

cbuffer CB_Projection
{
    row_major matrix Projection;
};

cbuffer CB_World
{
    row_major matrix World;
};

cbuffer CB_Light // 버텍스 쉐이더랑 별개이므로 다시 0번부터
{
    float4 LightColor;
    float3 LightDirection;
}

// material 재질 의미
cbuffer CB_Material
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

cbuffer PS_Lighting
{
    LightingData LightingDatas[6];

    int LightingCount;
}



//-----------------------------------------------------------------------------
// Textures
//-----------------------------------------------------------------------------
Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;
Texture2D DetailMap;



//-----------------------------------------------------------------------------
// Sampler States
//-----------------------------------------------------------------------------
//	DepthEnable = BOOL
//	DepthWriteMask = Zero | All
//	DepthFunc = Never | Less | Less_Equal | Greater | Not_Equal | Greater_Equal | Always
//	StencilEnable = BOOL
//	StencilReadMask = UINT8
//	StencilWriteMask = UINT8
//	FrontFaceStencilFail = Keep | Zero | Replace | Incr_Sat | Decr_Sat | Invert | Incr | Decr
//	FrontFaceStencilDepthFail = Keep | Zero | Replace | Incr_Sat | Decr_Sat | Invert | Incr | Decr
//	FrontFaceStencilPass = Keep | Zero | Replace | Incr_Sat | Decr_Sat | Invert | Incr | Decr
//	FrontFaceStencilFunc = Never | Less | Less_Equal | Greater | Not_Equal | Greater_Equal | Always
//	BackFaceStencilFail = Keep | Zero | Replace | Incr_Sat | Decr_Sat | Invert | Incr | Decr
//	BackFaceStencilDepthFail = Keep | Zero | Replace | Incr_Sat | Decr_Sat | Invert | Incr | Decr
//	BackFaceStencilPass = Keep | Zero | Replace | Incr_Sat | Decr_Sat | Invert | Incr | Decr
//	BackFaceStencilFunc = Never | Less | Less_Equal | Greater | Not_Equal | Greater_Equal | Always
//--------------------------------------------------------------------------------

SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

SamplerState BilinearSampler
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

SamplerState TrilinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

SamplerState AnisotropicSampler
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};


//-----------------------------------------------------------------------------
// Depth-Stencil States
//-----------------------------------------------------------------------------
//	DepthEnable = BOOL
//	DepthWriteMask = Zero | All
//	DepthFunc = Never | Less | Less_Equal | Greater | Not_Equal | Greater_Equal | Always
//	StencilEnable = BOOL
//	StencilReadMask = UINT8
//	StencilWriteMask = UINT8
//	FrontFaceStencilFail = Keep | Zero | Replace | Incr_Sat | Decr_Sat | Invert | Incr | Decr
//	FrontFaceStencilDepthFail = Keep | Zero | Replace | Incr_Sat | Decr_Sat | Invert | Incr | Decr
//	FrontFaceStencilPass = Keep | Zero | Replace | Incr_Sat | Decr_Sat | Invert | Incr | Decr
//	FrontFaceStencilFunc = Never | Less | Less_Equal | Greater | Not_Equal | Greater_Equal | Always
//	BackFaceStencilFail = Keep | Zero | Replace | Incr_Sat | Decr_Sat | Invert | Incr | Decr
//	BackFaceStencilDepthFail = Keep | Zero | Replace | Incr_Sat | Decr_Sat | Invert | Incr | Decr
//	BackFaceStencilPass = Keep | Zero | Replace | Incr_Sat | Decr_Sat | Invert | Incr | Decr
//	BackFaceStencilFunc = Never | Less | Less_Equal | Greater | Not_Equal | Greater_Equal | Always
//-----------------------------------------------------------------------------
// SetDepthStencilState( NoDepth, 0 );
//-----------------------------------------------------------------------------

DepthStencilState DepthStencilState_Default
{
    DepthEnable = true;
    StencilEnable = true;

    DepthFunc = Less_Equal;
    DepthWriteMask = All;

    StencilReadMask = 0xFF;
    StencilWriteMask = 0xFF;

    FrontFaceStencilFail = Keep;
    FrontFaceStencilDepthFail = Incr;
    FrontFaceStencilPass = Keep;
    FrontFaceStencilFunc = Always;

    BackFaceStencilFail = Keep;
    BackFaceStencilDepthFail = Decr;
    BackFaceStencilPass = Keep;
    BackFaceStencilFunc = Always;
};

DepthStencilState NoDepth
{
    DepthEnable = false;
    StencilEnable = true;

    DepthFunc = Less_Equal;
    DepthWriteMask = Zero;

    StencilReadMask = 0xFF;
    StencilWriteMask = 0xFF;

    FrontFaceStencilFail = Keep;
    FrontFaceStencilDepthFail = Incr;
    FrontFaceStencilPass = Keep;
    FrontFaceStencilFunc = Always;

    BackFaceStencilFail = Keep;
    BackFaceStencilDepthFail = Decr;
    BackFaceStencilPass = Keep;
    BackFaceStencilFunc = Always;
};

DepthStencilState NoDepthWriteMaskZero
{
    DepthEnable = false;
    StencilEnable = true;
    DepthWriteMask = Zero;

    StencilReadMask = 0xFF;
    StencilWriteMask = 0xFF;

    FrontFaceStencilFail = Keep;
    FrontFaceStencilDepthFail = Incr;
    FrontFaceStencilPass = Keep;
    FrontFaceStencilFunc = Always;

    BackFaceStencilFail = Keep;
    BackFaceStencilDepthFail = Decr;
    BackFaceStencilPass = Keep;
    BackFaceStencilFunc = Always;
};

DepthStencilState NoDepthNoStencil
{
    DepthEnable = false;
    StencilEnable = false;
};

DepthStencilState DepthNoStencil
{
    DepthEnable = true;
    DepthWriteMask = All;
    DepthFunc = LESS;
    StencilEnable = false;
};


//-----------------------------------------------------------------------------
// Blend States
//-----------------------------------------------------------------------------
//	AlphaToCoverageEnable = BOOL
//	BlendEnable[] = BOOL
//	SrcBlend[] = Zero | One | Src_Color | Inv_Src_Color | Src_Alpha | Inv_Src_Alpha | Dest_Alpha | Inv_Dest_Alpha | Dest_Color |
//				 Inv_Dest_Color | Src_Alpha_Sat | Blend_Factor | Inv_Blend_Factor | Src1_Color | Inv_Src1_Color | Src1_Alpha | Inv_Src1_Alpha
//	DestBlend[] = Zero | One | ...
//	BlendOp[] = Add | Subtract | Rev_Subtract | Min | Max
//	SrcBlendAlpha[] = Zero | One | ...
//	DestBlendAlpha[] = Zero | One | ...
//	BlendOpAlpha[] = Add | Subtract | Rev_Subtract | Min | Max
//	RenderTargetWriteMask[] = UINT8
//-----------------------------------------------------------------------------
// SetBlendState( AlphaBlend1, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF ); 
//-----------------------------------------------------------------------------

BlendState BlendEnable
{
    BlendEnable[0] = true;
    AlphaToCoverageEnable = false;

    DestBlend[0] = INV_SRC_ALPHA;
    SrcBlend[0] = SRC_ALPHA;
    BlendOp[0] = Add;

    DestBlendAlpha[0] = Zero;
    SrcBlendAlpha[0] = Zero;
    BlendOpAlpha[0] = Add;

    RenderTargetWriteMask[0] = 1 | 2 | 4 | 8;
};

BlendState BlendDisable
{
    AlphaToCoverageEnable = false;
    BlendEnable[0] = false;
};

BlendState AlphaBlend1
{
    BlendEnable[0] = true;
    BlendOp[0] = Add;
    BlendOpAlpha[0] = Add;
    DestBlend[0] = Inv_Src_Alpha;
    DestBlendAlpha[0] = Zero;
    SrcBlend[0] = Src_Alpha;
    SrcBlendAlpha[0] = One;
    RenderTargetWriteMask[0] = 1 | 2 | 4 | 8; // Write all colors R | G | B | A
};

BlendState AlphaBlend2
{
    AlphaToCoverageEnable = false;
    BlendEnable[0] = true;

    BlendOP[0] = Add;
    DestBlend[0] = One;
    SrcBlend[0] = Src_Alpha;

    BlendOpAlpha[0] = Add;
    DestBlendAlpha[0] = Zero;
    SrcBlendAlpha[0] = Zero;

    RenderTargetWriteMask[0] = 1 | 2 | 4 | 8; // Write all colors R | G | B | A
};

BlendState ColorAccumulationBlend
{
    BlendEnable[0] = true;
    BlendOp[0] = Add;
    BlendOpAlpha[0] = Add;
    DestBlend[0] = One;
    DestBlendAlpha[0] = One;
    SrcBlend[0] = One;
    SrcBlendAlpha[0] = One;
    RenderTargetWriteMask[0] = 1 | 2 | 4 | 8;
};

BlendState ColorSubtractionBlend
{
    BlendEnable[0] = true;
    BlendOp[0] = Subtract;
    BlendOpAlpha[0] = Add;
    DestBlend[0] = One;
    DestBlendAlpha[0] = One;
    SrcBlend[0] = One;
    SrcBlendAlpha[0] = One;
    RenderTargetWriteMask[0] = 1 | 2 | 4 | 8;
};





//-----------------------------------------------------------------------------
// Rasterizer States
//-----------------------------------------------------------------------------
//	FillMode = Solid | Wireframe;
//	CullMode = None | Front | Back;
//	FrontCounterClockwise = true | false;
//	DepthBias = INT;
//	DepthClipEnable = BOOL;
//	DepthBiasClamp = BOOL;
//	SlopeScaledDepthBias = FLOAT;
//	ScissorEnable = BOOL;
//	MultisampleEnable = BOOL;
//	AntialiasedLineEnable = BOOL;
//-----------------------------------------------------------------------------
// SetRasterizerState( SolidCullNone );
//-----------------------------------------------------------------------------

RasterizerState SolidCullNone
{
    FillMode = Solid;
    CullMode = None;
    DepthClipEnable = true;
    FrontCounterClockwise = false;
    MultisampleEnable = false;
    ScissorEnable = false;
};

RasterizerState RasterizerState_Default
{
    FillMode = Solid;
    CullMode = Back;
    DepthClipEnable = true;
    DepthBias = 0;
    FrontCounterClockwise = false;
    MultisampleEnable = true;
    ScissorEnable = false;
    SlopeScaledDepthBias = 0;
};

RasterizerState SolidNoMS
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
    MultisampleEnable = false;
};

RasterizerState SolidNoCullNoMS
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockwise = false;
    MultisampleEnable = false;
};

RasterizerState SolidMS
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
    MultisampleEnable = true;
};

RasterizerState SolidNoCullMS
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockwise = false;
    MultisampleEnable = true;
};

RasterizerState WireNoAA
{
    FillMode = Wireframe;
    CullMode = Back;
    FrontCounterClockwise = false;
    AntialiasedLineEnable = false;
};

RasterizerState WireNoCullNoAA
{
    FillMode = Wireframe;
    CullMode = None;
    FrontCounterClockwise = false;
    AntialiasedLineEnable = false;
};

RasterizerState WireAA
{
    FillMode = Wireframe;
    CullMode = Back;
    FrontCounterClockwise = false;
    AntialiasedLineEnable = true;
};

RasterizerState WireNoCullAA
{
    FillMode = Wireframe;
    CullMode = None;
    FrontCounterClockwise = false;
    AntialiasedLineEnable = true;
};

//// State array pattern

////RasterizerState states[2] {
// //{FillMode = Solid; CullMode = Back;}, 
// //{FillMode = Wireframe; CullMode = Front;}
////};


//-----------------------------------------------------------------------------
// Vertex Layout
//-----------------------------------------------------------------------------
struct Vertex
{
    float4 Position : POSITION0;
};

struct VertexSize
{
    float4 Position : POSITION0;
    float2 Size : SIZE0;
};

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



//-----------------------------------------------------------------------------
// Bones
//-----------------------------------------------------------------------------
int BoneIndex;
cbuffer CB_Bones
{
    matrix Bones[128];
};

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


//-----------------------------------------------------------------------------
// Light
//-----------------------------------------------------------------------------
struct Material
{
    float3 Normal;
    float4 DiffuseColor;
    float4 SpecularColor; // 이것의 a를 강도로 씀
    float Shininess;
    //float3 vNormal;
};

float3 CreateNormalWithNormalMapping(float4 wPosition, float2 uv, float3 vertexNormal, float3 tangent, out float3 binorm)
{
    float4 normalMap = NormalMap.Sample(TrilinearSampler, uv);
    float3 norm = float3(normalMap.g * 2.0f - 1.0f, normalMap.a * 2.0f - 1.0f, 0);
    norm.z = sqrt(1.0f - norm.x * norm.x + norm.y * norm.y);
    binorm = normalize(cross(vertexNormal, tangent));
    //binorm = (dot(normalize(wPosition.xyz), binorm) < 0) ? -binorm : binorm;
    float3x3 BTNMatrix = float3x3(binorm, tangent, vertexNormal);
    float3 N = normalize(mul(norm, BTNMatrix));

    return N;
}

Material CreateMaterial(float3 normal, float2 uv)
{
    Material material;
    material.Normal = normalize(normal);
    material.DiffuseColor = DiffuseMap.Sample(TrilinearSampler, uv);
    material.DiffuseColor = (length(material.DiffuseColor) <= 0.0001f) ? Diffuse : material.DiffuseColor;
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
    
    //color *= material.DiffuseColor;

    float EnvIntencity = saturate(dot(material.Normal, -LightDirection));
    //EnvIntencity = (dot(material.Normal, Direction) > 0) ? 0 : EnvIntencity;
    
    color = material.DiffuseColor.xyz * saturate(EnvIntencity + color);
    
    return color;
}
