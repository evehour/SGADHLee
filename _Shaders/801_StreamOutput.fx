#include "000_Header.fx"

///////////////////////////////////////////////////////////////////////////////
//VS part

VertexColor VS(VertexColor input)
{
    VertexColor output;
    output.Position = input.Position;
    output.Color = input.Color;
    return output;
}

VertexColor VSPassThrough(VertexColor input)
{
    return input;
}

///////////////////////////////////////////////////////////////////////////////
//GS part

struct GeometryOutput
{
    float4 Position : SV_POSITION0;
    float4 Color : COLOR0;
};

//[maxvertexcount(1)]
//void gsStreamOut(point VertexColor input[1], inout PointStream<VertexColor> SpriteStream)
//{
//    VertexColor output;
//    output.Position = input[0].Position;
//    output.Color = input[0].Color;
//    SpriteStream.Append(output);
//}

// GS for rendering rain as point sprites.  Takes a point and turns it into 2 tris.
[maxvertexcount(3)]
void GS(point VertexColor input[1], inout TriangleStream<GeometryOutput> SpriteStream)
{
    GeometryOutput output;

    float3 up = float3(0, 1, 0);
    float3 look = input[0].Position.xyz - ViewPosition;
    look.y = 0.0f; // 수직고정
    look = normalize(look);

    float3 right = cross(up, look);
    
    float halfWidth = 1.0f * 0.5f;
    float halfHeight = 1.0f * 0.5f;

    float4 v[3];
    // Billboard
    //v[0] = float4(input[0].Position.xyz - halfWidth * right - halfHeight * up, 1.0f);
    //v[1] = float4(input[0].Position.xyz + halfHeight * up, 1.0f);
    //v[2] = float4(input[0].Position.xyz + halfWidth * right - halfHeight * up, 1.0f);

    // Defualt
    v[0] = float4(input[0].Position.xyz - float3(halfWidth, halfHeight, 0), 1.0f);
    v[1] = float4(input[0].Position.xyz + float3(0, halfHeight, 0), 1.0f);
    v[2] = float4(input[0].Position.xyz - float3(-halfWidth, halfHeight, 0), 1.0f);

    
    [unroll]
    for (int i = 0; i < 3; ++i)
    {
        output.Position = mul(v[i], View);
        output.Position = mul(output.Position, Projection);

        //output.Color = float4(0, 0, 0, 1);
        //output.Color.r = (i == 0) ? 1 : 0;
        //output.Color.g = (i == 1) ? 1 : 0;
        //output.Color.b = (i == 2) ? 1 : 0;
        output.Color = input[0].Color;
        SpriteStream.Append(output);
    }
    SpriteStream.RestartStrip();
}

///////////////////////////////////////////////////////////////////////////////
//PS part

float4 PS(GeometryOutput input) : SV_TARGET
{
    return input.Color;
}

VertexShader vsShader = CompileShader(vs_5_0, VS());
GeometryShader gsStreamOut = ConstructGSWithSO(vsShader, "0:POSITION.xyzw; 0:COLOR.xyzw");

technique11 T0
{
    pass P0
    {
		// 스트림에만 출력을 하고 랜더링 하지 않는 작업에서는
		// 깊이버퍼(마스크는 제로)를 꺼야 한다.
        SetDepthStencilState(NoDepthWriteMaskZero, 0);

        SetVertexShader(vsShader);
        SetGeometryShader( gsStreamOut );
        SetPixelShader(NULL);
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VSPassThrough()));
        SetGeometryShader(CompileShader(gs_5_0, GS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}

