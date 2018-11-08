#include "000_Header2.hlsl"

struct VertexOutput
{
    float3 Center : POSITION;
    float2 Size : Size;
};

VertexOutput VS(VertexSize input)
{
    VertexOutput output;

    output.Center = input.Position.xyz;
    output.Size = input.Size;

    return output;
}

///////////////////////////////////////////////////////////////////////////////

struct GeometryOutput
{
    float4 Position : SV_POSITION;
    float3 wPosition : POSITION0;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;

    float3 cPosition : POSITION1;
};

static const float2 TexCoord[4] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(1.0f, 0.0f),
};

[maxvertexcount(4)]
void GS(point VertexOutput input[1], inout TriangleStream<GeometryOutput> stream)
{
    float3 up = float3(0, 1, 0);
    float3 look = CameraPosition() - input[0].Center;
    look.y = 0.0f; // 수직고정
    look = normalize(look);

    float3 right = cross(up, look);
    
    float halfWidth = 0.5f * input[0].Size.x;
    float halfHeight = 0.5f * input[0].Size.y;

    float4 v[4];
    v[0] = float4(input[0].Center + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(input[0].Center + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(input[0].Center - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(input[0].Center - halfWidth * right + halfHeight * up, 1.0f);

    GeometryOutput output;
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        output.Position = mul(v[i], View);
        output.Position = mul(output.Position, Projection);
        output.wPosition = v[i].xyz;

        output.Normal = look;
        output.Uv = TexCoord[i];
        output.cPosition = CameraPosition();

        stream.Append(output);
    }

}

///////////////////////////////////////////////////////////////////////////////

Texture2D Map : register(t10);
SamplerState Sampler : register(s10);

float4 PS(GeometryOutput input) : SV_TARGET
{
    // SV_가 붙은것은 무조건 바뀐다.
    // 그 외에 것들은 VS에서 RS로 인해 노멀라이즈한것이 바뀔 수도 있다.
    // 때문에 픽셀쉐이더에서 노멀라이즈 하는것이 확실하고 그렇게 하자.
    //input.Normal = normalize(input.Normal);
    
    float4 color = Map.Sample(Sampler, input.Uv);
    clip(color.a - 0.5f);

    return color;
}