#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float4 dPosition : POSITION0; // ���̰� �� ��ġ
};

PixelInput VS_Depth(VertexTextureNormalTangentBlend input)
{
    PixelInput output;

    //matrix world = BoneWorld();
    matrix world = 0;

   [branch]
    if (UseBlend == true)
        world = SkinWorld(input.BlendIndices, input.BlendWeights);
    else
        world = BoneWorld();

    output.Position = mul(input.Position, world);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.dPosition = output.Position;

    return output;
}

float4 PS_Depth(PixelInput input) : SV_TARGET
{
    float depth = input.dPosition.z / input.dPosition.w;

    return float4(depth, depth, depth, 1);
}

///////////////////////////////////////////////////////////////////////////////

// Shadow Shader

cbuffer VS_Light : register(b10)
{
    matrix V;
    matrix P;
}

struct PixelInputShadow
{
    float4 Position : SV_POSITION0;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
    float4 vPosition : LIGHTVIEW0;
};

PixelInputShadow VS_Shadow(VertexTextureNormalTangentBlend input)
{
    PixelInputShadow output;

    matrix world = 0;

   [branch]
    if (UseBlend == true)
        world = SkinWorld(input.BlendIndices, input.BlendWeights);
    else
        world = BoneWorld();

    output.Position = mul(input.Position, world);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.vPosition = mul(input.Position, world);
    output.vPosition = mul(output.vPosition, V);
    output.vPosition = mul(output.vPosition, P);

    output.Normal = WorldNormal(input.Normal, world);
    output.Uv = input.Uv;

    return output;
}

cbuffer PS_Shadow : register(b10)
{
    float2 MapSize;

    float Bias;
    int Selected;
}

Texture2D DepthMap : register(t10); // ���� ���� render2D 
SamplerComparisonState DepthSampler : register(s10);

float4 PS_Shadow(PixelInputShadow input) : SV_TARGET
{
    input.vPosition.xyz /= input.vPosition.w;

    float4 color = 0;
    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    DiffuseLighting(color, diffuse, input.Normal);

    if (input.vPosition.x < -1.0f || input.vPosition.x > 1.0f ||
        input.vPosition.y < -1.0f || input.vPosition.y > 1.0f ||
        input.vPosition.z < 0.0f || input.vPosition.z > 1.0f)
    {
        return color;
    }
    else
    {
        input.vPosition.x = input.vPosition.x * 0.5f + 0.5f;
        input.vPosition.y = -input.vPosition.y * 0.5f + 0.5f;
        input.vPosition.z -= Bias; // �̼��� �� �ذ� zFighting ���� ������

        float depth = 0;
        float factor = 0;

       [Branch]
        if (Selected == 0)
        {
            depth = DepthMap.Sample(DiffuseSampler, input.vPosition.xy).r;
          // ���̰� �۰ų� ���ٴ� ���� ����� ���̶�� ��� -> �� ������ true
            factor = (float) input.vPosition.z <= depth;
        }
        else if (Selected == 1)
        {
            depth = DepthMap.SampleCmpLevelZero(DepthSampler, input.vPosition.xy, input.vPosition.z);
                // ���̰� �۰ų� ���ٴ� ���� ����� ���̶�� ��� -> �� ������ true
            factor = (float) input.vPosition.z <= depth;
        }
        else if (Selected == 2)
        {
            float sum = 0;
            float avg = 0;
          // ��ġ�� ���߿� ���۷� �ѱ�� ��
            //for (float y = -1.5f; y <= 1.5f; y += 1.0f)
            //{
            //    for (float x = -1.5f; x <= 1.5f; x += 1.0f)
            //    {
            //        float2 offset = float2(x * 1.0f / MapSize.x, y * 1.0f / MapSize.y);
            
            //        sum += DepthMap.SampleCmpLevelZero(DepthSampler, input.vPosition.xy + offset, input.vPosition.z).r;
          //        avg++;
            //    }
            //}

          // �������� �ص� �� ���ɿ� ���� �ϴ� �ɷ�
            for (float y = -1.5f; y <= 1.5f; y += 1.0f)
            {
                for (float x = -1.5f; x <= 1.5f; x += 1.0f)
                {
                    float2 offset = float2(x * 1.0f / MapSize.x, y * 1.0f / MapSize.y);
            
                    sum += DepthMap.SampleCmpLevelZero(DepthSampler, input.vPosition.xy, input.vPosition.z).r;
                    avg++;
                }
            }

            //factor = sum / 16.0f; // 4x4 �� 16���� ������
            factor = sum / avg; // avg�� ������ ��
        }

        color.rgb *= factor;

        return float4(color.rgb, 1);
    }
}