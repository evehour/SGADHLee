
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
    float4 LightAmbient;
    float4 LightDiffuse;
    float4 LightSpecular;
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

SamplerState PointSamplerClamp
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
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

    DepthFunc = Less_Equal; // Less_Equal : 작거나 같은 것을 그리는 것.
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
    StencilEnable = false;
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

BlendState AlphaBlend3
{
    AlphaToCoverageEnable = true;
    RenderTargetWriteMask[0] = 1 | 2 | 4 | 8;
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

struct VertexTextureNormalIndex
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float2 Index : INDEX0;
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
// Global Functions
//-----------------------------------------------------------------------------
float3 WorldNormal(float3 normal)
{
    return normalize(mul(normal, (float3x3) World));
}

float3 NormalMapping(float4 normalMap, float3 normal, float3 tangent)
{
    float3 N = normal; //Z축
    float3 T = normalize(tangent - dot(tangent, N) * N); // X축 그람-슈미츠
    float3 B = cross(N, T); //Y축

    float3x3 TBN = float3x3(T, B, N);

    float3 coord = 2.0f * normalMap - 1.0f;
    float3 bump = mul(coord, TBN); // bumpMapping == normalMapping 용어가 같음.

    return bump;
}


//-----------------------------------------------------------------------------
// Lighting
//-----------------------------------------------------------------------------
struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float Shininess;
};

//-----------------------------------------------------------------------------
// Directional Lighting
//-----------------------------------------------------------------------------
struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
};

void ComputeDirectionalLight(Material m, DirectionalLight l, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 specular)
{
    ambient = float4(0, 0, 0, 0);
    diffuse = float4(0, 0, 0, 0);
    specular = float4(0, 0, 0, 0);

    float3 light = -l.Direction;
    ambient = m.Ambient * l.Ambient;

    float diffuseFactor = dot(light, normal);

    [flatten]
    if (diffuseFactor > 0.0f)
    {
        diffuse = diffuseFactor * m.Diffuse * l.Diffuse;

        
        float3 r = reflect(-light, normal);
        
        float specularFactor = 0;
        specularFactor = saturate(dot(r, toEye));
        specularFactor = pow(specularFactor, m.Specular.a);
        specular = specularFactor * m.Specular * l.Specular;
    }
}

//-----------------------------------------------------------------------------
// Point Lighting
//-----------------------------------------------------------------------------
struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float Range;

    float3 Attenuation;
    float PointLight_Padding;
};

cbuffer CB_PointLight
{
    PointLight PointLights[16];
    int PointLightCount;
};

void ComputePointLight(Material m, PointLight l, float3 position, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 specular)
{
    ambient = float4(0, 0, 0, 0);
    diffuse = float4(0, 0, 0, 0);
    specular = float4(0, 0, 0, 0);

    float3 light = l.Position - position;
    float dist = length(light);
    
    if (dist > l.Range)
        return;

    light /= dist;
    ambient = m.Ambient * l.Ambient;

    float diffuseFactor = dot(light, normal);

    [flatten]
    if (diffuseFactor > 0.0f)
    {
        diffuse = diffuseFactor * m.Diffuse * l.Diffuse;

        float3 r = reflect(-light, normal);
        
        float specularFactor = 0;
        specularFactor = saturate(dot(r, toEye));
        specularFactor = pow(specularFactor, m.Specular.a);
        specular = specularFactor * m.Specular * l.Specular;
    }


    float attenuate = 1.0f / dot(l.Attenuation, float3(1.0f, dist, dist * dist));

    diffuse *= attenuate;
    specular *= attenuate;
}

//-----------------------------------------------------------------------------
// Spot Lighting
//-----------------------------------------------------------------------------
struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float PointLight_Padding;

    float3 Direction;
    float Spot;

    float3 Attenuation;
    float PointLight_Padding2;
};

cbuffer CB_SpotLight
{
    SpotLight SpotLights[16];
    int SpotLightCount;
};

void ComputeSpotLight(Material m, SpotLight l, float3 position, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 specular)
{
    ambient = float4(0, 0, 0, 0);
    diffuse = float4(0, 0, 0, 0);
    specular = float4(0, 0, 0, 0);

    float3 light = l.Position - position;
    float dist = length(light);

    light /= dist;
    ambient = m.Ambient * l.Ambient;

    float diffuseFactor = dot(light, normal);

    [flatten]
    if (diffuseFactor > 0.0f)
    {
        diffuse = diffuseFactor * m.Diffuse * l.Diffuse;

        float3 r = reflect(-light, normal);
        
        float specularFactor = 0;
        specularFactor = saturate(dot(r, toEye));
        specularFactor = pow(specularFactor, m.Specular.a);
        specular = specularFactor * m.Specular * l.Specular;
    }


    float spot = pow(max(dot(-light, l.Direction), 0.0f), l.Spot);
    float attenuate = spot / dot(l.Attenuation, float3(1.0f, dist, dist * dist));

    ambient *= attenuate;
    diffuse *= attenuate;
    specular *= attenuate;
}

//-----------------------------------------------------------------------------
// Math
//-----------------------------------------------------------------------------
float PlaneDot(float4 plane, float4 var)
{
    return plane.x * var.x + plane.y * var.y, plane.z * var.z + plane.w * var.w;
}
//-----------------------------------------------------------------------------
// Frustum
//-----------------------------------------------------------------------------
float4 FrustumWall[6];

bool ContainCube(float3 position, float radius)
{
    float3 check = 0;

    [unroll]
    for (int i = 0; i < 6; i++)
    {
        check.x = position.x - radius;
        check.y = position.y - radius;
        check.z = position.z - radius;
        [branch]
        if (PlaneDot(FrustumWall[i], float4(check, 1)) >= 0.0f)
            continue;
        
        check.x = position.x + radius;
        check.y = position.y - radius;
        check.z = position.z - radius;
        [branch]
        if (PlaneDot(FrustumWall[i], float4(check, 1)) >= 0.0f)
            continue;
        
        check.x = position.x - radius;
        check.y = position.y + radius;
        check.z = position.z - radius;
        [branch]
        if (PlaneDot(FrustumWall[i], float4(check, 1)) >= 0.0f)
            continue;
        
        check.x = position.x + radius;
        check.y = position.y + radius;
        check.z = position.z - radius;
        [branch]
        if (PlaneDot(FrustumWall[i], float4(check, 1)) >= 0.0f)
            continue;
        
        check.x = position.x - radius;
        check.y = position.y - radius;
        check.z = position.z + radius;
        [branch]
        if (PlaneDot(FrustumWall[i], float4(check, 1)) >= 0.0f)
            continue;
        
        check.x = position.x + radius;
        check.y = position.y - radius;
        check.z = position.z + radius;
        [branch]
        if (PlaneDot(FrustumWall[i], float4(check, 1)) >= 0.0f)
            continue;
        
        check.x = position.x - radius;
        check.y = position.y + radius;
        check.z = position.z + radius;
        [branch]
        if (PlaneDot(FrustumWall[i], float4(check, 1)) >= 0.0f)
            continue;
        
        check.x = position.x + radius;
        check.y = position.y + radius;
        check.z = position.z + radius;
        [branch]
        if (PlaneDot(FrustumWall[i], float4(check, 1)) >= 0.0f)
            continue;

        return false;
    }

    return true;
}