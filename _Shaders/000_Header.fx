
//-----------------------------------------------------------------------------
// CBuffer
//-----------------------------------------------------------------------------
cbuffer CB_PerFrame
{
    matrix View;

    float3 ViewDirection;
    float Time;
    float3 ViewPosition;
};

cbuffer CB_Projection
{
    matrix Projection;
};

cbuffer CB_World
{
    matrix World;
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
// SetDepthStencilState( DepthNoStencil, 0 );
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
// SetBlendState( BlendDisable, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF ); 
//-----------------------------------------------------------------------------

BlendState BlendDisable
{
    AlphaToCoverageEnable = false;
    BlendEnable[0] = false;
};

BlendState AlphaBlend
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
// SetRasterizerState( SolidNoMS );
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


