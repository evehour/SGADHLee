#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
    float4 wPosition : POSITION0;
    float3 cPosition : POSITION1;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    matrix world = BoneWorld();
    output.Position = mul(input.Position, world);
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;

    output.Normal = WorldNormal(input.Normal, world);
    output.cPosition = CameraPosition();

    return output;
}

cbuffer PS_Ambient : register(b10)
{
    float4 DirectionColor;

    float3 AmbientFloor; // : packoffset(c0); // 4����Ʈ ����� �˾Ƽ� �������� ����̷�.
    float PS_Ambient_Padding;

    float3 AmbientCeil;
    float PS_Ambient_Padding2;
}

cbuffer PS_Specular : register(b11)
{
    float SpecularExp;
    float SpecularIntensity;
}

cbuffer PS_Point : register(b12)
{
    float3 PointLightPosition;
    float PointLightRange;
    float4 PointLightColor;
}

cbuffer PS_Spot : register(b13)
{
    float4 SpotLightColor;

    float3 SpotLightPosition;
    float SpotLightRange;

    float3 SpotLightDirectionToLight;
    float SpotLightOuter;

    float SpotLightInner;
}

cbuffer PS_Capsule : register(b9)
{
    float4 CapsuleLightColor;

    float3 CapsuleLightPosition;
    float CapsuleLightRange;
    
    float3 CapsuleLightDirection;
    float CapsuleLightLength;
}

struct Material
{
    float3 Normal;
    float4 DiffuseColor;
    float SpecularExp;
    float SpecularIntensity;
};

float3 CalcAmbient(float3 normal, float3 color)
{
    float up = normal.y * 0.5f + 0.5f;
    float3 ambient = AmbientFloor + up * AmbientCeil; // ������ �߿��� �̰� �� ����.

    return ambient * color;
}

float3 CalcDirectional(float3 wPosition, float3 cPosition, Material material)
{
    float d = dot(-Direction, material.Normal);
    float3 color = DirectionColor.rgb * saturate(d);

    float3 toEye = normalize(cPosition - wPosition);
    float3 halfWay = normalize(toEye + (-Direction)); // �ݻ�� ��� ���͸� �̿��ؼ� �ݻ纤�͸� �̿��ߴ�. �̰��� ����� toEye�� �̿��� ��ֹ���(���簪)�� ����°Ŵ�.
    float h = saturate(dot(halfWay, material.Normal));
    color += DirectionColor.rgb * pow(h, material.SpecularExp) * material.SpecularIntensity;

    return color * material.DiffuseColor.rgb;
}

float3 CalcPoint(float3 wPosition, float3 cPosition, Material material)
{
    // PointLight
    float3 toLight = PointLightPosition.xyz - wPosition;
    float distanceToLight = length(toLight); // ������ ũ�Ⱑ �Ÿ�
    toLight /= distanceToLight; // ����ȭ�ϴ°� normalize�� ��������.

    // Diffuse
    float d = dot(toLight, material.Normal);
    float3 color = PointLightColor.rgb * saturate(d);

    //Specular
    float3 toEye = normalize(cPosition - wPosition);
    float3 halfWay = normalize(toEye + toLight); // �ݻ�� ��� ���͸� �̿��ؼ� �ݻ纤�͸� �̿��ߴ�. �̰��� ����� toEye�� �̿��� ��ֹ���(���簪)�� ����°Ŵ�.
    float h = saturate(dot(halfWay, material.Normal));
    color += PointLightColor.rgb * pow(h, material.SpecularExp) * material.SpecularIntensity;
    
    //Attenuation (����)
    float distanceToLightNormal = 1.0f - saturate(distanceToLight * 1.0f / PointLightRange);
    float attention = distanceToLightNormal * distanceToLightNormal; // ���������� ��
    color *= material.DiffuseColor * attention;

    return color;
}

float3 CalcSpot(float3 wPosition, float3 cPosition, Material material)
{
    // PointLight
    float3 toLight = SpotLightPosition.xyz - wPosition;
    float distanceToLight = length(toLight); // ������ ũ�Ⱑ �Ÿ�
    toLight /= distanceToLight; // ����ȭ�ϴ°� normalize�� ��������.

    // Diffuse
    float d = dot(toLight, material.Normal);
    float3 color = SpotLightColor.rgb * saturate(d);

    //Specular
    float3 toEye = normalize(cPosition - wPosition);
    float3 halfWay = normalize(toEye + toLight); // �ݻ�� ��� ���͸� �̿��ؼ� �ݻ纤�͸� �̿��ߴ�. �̰��� ����� toEye�� �̿��� ��ֹ���(���簪)�� ����°Ŵ�.
    float h = saturate(dot(halfWay, material.Normal));
    color += SpotLightColor.rgb * pow(h, material.SpecularExp) * material.SpecularIntensity;

    //Attenuation (����) ��������
    float3 dir = -normalize(SpotLightDirectionToLight);
    float cosAngle = dot(dir, toLight);

    float outer = cos(radians(SpotLightOuter));
    float inner = 1.0f / cos(radians(SpotLightInner));

    float coneAttention = saturate((cosAngle - outer) * inner);
    coneAttention *= coneAttention;
    
    float distanceToLightNormal = 1.0f - saturate(distanceToLight * 1.0f / SpotLightRange);
    float attention = distanceToLightNormal * distanceToLightNormal; // ���������� ��
    color *= material.DiffuseColor * attention * coneAttention;

    return color;
}

float3 CalcCapsule(float3 wPosition, float3 cPosition, Material material)
{
    // PointLight
    float3 dir = normalize(CapsuleLightDirection);
    float3 start = wPosition - CapsuleLightPosition;
    float distanceOnLine = dot(start, dir) / CapsuleLightLength; // ������ ����.
    distanceOnLine = saturate(distanceOnLine) * CapsuleLightLength; // �������� �� ���� ������ ���̷� ���ϴ� ������ ��ġ�� �����ϱ� ����(0~1. 0�̸��̳� 1�ʰ��� ���ֱ� ����.)

    float3 pointOnLine = CapsuleLightPosition + dir * distanceOnLine; // distanceOnLine�� �� �ɰ� ������ ������ ����
    
    //------> ��������� PointLight�� wPosition�� ���ϱ� ���� ��.
    //------> �� ���ĺ��ʹ� ����Ʈ����Ʈ�� ���Ͽ� ���� �������Ѽ� A ���� B �׸�.

    float3 toLight = pointOnLine - wPosition;
    float distanceToLight = length(toLight);

    toLight /= distanceToLight;

    // Diffuse
    float d = dot(toLight, material.Normal);
    float3 color = material.DiffuseColor.rgb * saturate(d);

    //Specular
    float3 toEye = normalize(cPosition - wPosition);
    float3 halfWay = normalize(toEye + toLight); // �ݻ�� ��� ���͸� �̿��ؼ� �ݻ纤�͸� �̿��ߴ�. �̰��� ����� toEye�� �̿��� ��ֹ���(���簪)�� ����°Ŵ�.
    float h = saturate(dot(halfWay, material.Normal));
    color += pow(h, material.SpecularExp) * material.SpecularIntensity;

    //Attenuation
    float distanceToLightNormal = 1.0f - saturate(distanceToLight * 1.0f / CapsuleLightRange);
    float attention = distanceToLightNormal * distanceToLightNormal; // ���������� ��
    color *= CapsuleLightColor.rgb * attention;

    return color;
}

Material CreateMaterial(float3 normal, float2 uv)
{
    Material material;
    material.Normal = normalize(normal);
    
    //material.DiffuseColor = DiffuseMap.Sample(DiffuseSampler, uv);
    material.DiffuseColor = Diffuse;
    material.DiffuseColor.rgb *= material.DiffuseColor.rgb;

    // �Ҹ� ��
    material.SpecularExp = SpecularExp;
    material.SpecularIntensity = SpecularIntensity;

    return material;
}

//CapsuleLight
float4 PS(PixelInput input) : SV_TARGET
{
    Material material = CreateMaterial(input.Normal, input.Uv);
    float4 color = 1;

    color.rgb *= CalcCapsule(input.wPosition.xyz, input.cPosition, material);
    color.a = 1.0f;

    return color;
}

//SpotLight
float4 PS_Spot(PixelInput input) : SV_TARGET
{
    Material material = CreateMaterial(input.Normal, input.Uv);
    float4 color = 1;

    color.rgb *= CalcSpot(input.wPosition.xyz, input.cPosition, material);
    color.a = 1.0f;

    return color;
}

//PointLight
float4 PS_PointLight(PixelInput input) : SV_TARGET
{
    Material material = CreateMaterial(input.Normal, input.Uv);
    float4 color = 1;

    color.rgb *= CalcPoint(input.wPosition.xyz, input.cPosition, material);
    color.a = 1.0f;

    return color;
}

//Directional
float4 PS_Directional(PixelInput input) : SV_TARGET
{
    Material material = CreateMaterial(input.Normal, input.Uv);
    float4 color = 0;
    color.rgb = CalcAmbient(material.Normal, material.DiffuseColor.rgb);
    color.rgb *= CalcDirectional(input.wPosition.xyz, input.cPosition, material);
    color.a = 1.0f;

    return color;
}

//Ambient
float4 PS_Ambient(PixelInput input) : SV_TARGET
{
    float3 normal = normalize(input.Normal);
    float4 color = float4(Diffuse.rgb * Diffuse.rgb, Diffuse.a); // ���⼭�� alpha�� ������ �ƴ϶� ����.

    
    color.rgb = CalcAmbient(normal, color.rgb);
    color.a = 1.0f;

    return color;
}
