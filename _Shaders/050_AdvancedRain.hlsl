#include "000_Header2.hlsl"

#define PI 3.14159265

static const float g_heightRange = 30.0;
static const float g_PointLightIntensity = 2.0;
static const float g_ResponseDirLight = 1.0;
static const float g_ResponsePointLight = 1.0;

static const float g_SpriteSize = 1.0;
static const float dirLightIntensity = 1.0;
    
static const float4 pointLightColor = float4(1.0, 1.0, 1.0, 1.0);
static const float3 g_PointLightPos = float3(3.7, 5.8, 3.15);
static const float3 g_PointLightPos2 = float3(-3.7, 5.8, 3.15);

static const bool g_useSpotLight = true;
static const float g_cosSpotlightAngle = 0.8;
static const float3 g_SpotLightDir = float3(0, -1, 0);

static const float g_Near = 1.0f;
static const float g_Far = 1000.0f;

static const float2 g_texcoords[4] =
{
    float2(0, 1),
    float2(1, 1),
    float2(0, 0),
    float2(1, 0),
};
    
//normalization factors for the rain textures, one per texture
static const float g_rainfactors[370] =
{
    0.004535, 0.014777, 0.012512, 0.130630, 0.013893, 0.125165, 0.011809, 0.244907, 0.010722, 0.218252,
    0.011450, 0.016406, 0.015855, 0.055476, 0.015024, 0.067772, 0.021120, 0.118653, 0.018705, 0.142495,
    0.004249, 0.017267, 0.042737, 0.036384, 0.043433, 0.039413, 0.058746, 0.038396, 0.065664, 0.054761,
    0.002484, 0.003707, 0.004456, 0.006006, 0.004805, 0.006021, 0.004263, 0.007299, 0.004665, 0.007037,
    0.002403, 0.004809, 0.004978, 0.005211, 0.004855, 0.004936, 0.006266, 0.007787, 0.006973, 0.007911,
    0.004843, 0.007565, 0.007675, 0.011109, 0.007726, 0.012165, 0.013179, 0.021546, 0.013247, 0.012964,
    0.105644, 0.126661, 0.128746, 0.101296, 0.123779, 0.106198, 0.123470, 0.129170, 0.116610, 0.137528,
    0.302834, 0.379777, 0.392745, 0.339152, 0.395508, 0.334227, 0.374641, 0.503066, 0.387906, 0.519618,
    0.414521, 0.521799, 0.521648, 0.498219, 0.511921, 0.490866, 0.523137, 0.713744, 0.516829, 0.743649,
    0.009892, 0.013868, 0.034567, 0.025788, 0.034729, 0.036399, 0.030606, 0.017303, 0.051809, 0.030852,
    0.018874, 0.027152, 0.031625, 0.023033, 0.038150, 0.024483, 0.029034, 0.021801, 0.037730, 0.016639,
    0.002868, 0.004127, 0.133022, 0.013847, 0.123368, 0.012993, 0.122183, 0.015031, 0.126043, 0.015916,
    0.002030, 0.002807, 0.065443, 0.002752, 0.069440, 0.002810, 0.081357, 0.002721, 0.076409, 0.002990,
    0.002425, 0.003250, 0.003180, 0.011331, 0.002957, 0.011551, 0.003387, 0.006086, 0.002928, 0.005548,
    0.003664, 0.004258, 0.004269, 0.009404, 0.003925, 0.009233, 0.004224, 0.009405, 0.004014, 0.008435,
    0.038058, 0.040362, 0.035946, 0.072104, 0.038315, 0.078789, 0.037069, 0.077795, 0.042554, 0.073945,
    0.124160, 0.122589, 0.121798, 0.201886, 0.122283, 0.214549, 0.118196, 0.192104, 0.122268, 0.209397,
    0.185212, 0.181729, 0.194527, 0.420721, 0.191558, 0.437096, 0.199995, 0.373842, 0.192217, 0.386263,
    0.003520, 0.053502, 0.060764, 0.035197, 0.055078, 0.036764, 0.048231, 0.052671, 0.050826, 0.044863,
    0.002254, 0.023290, 0.082858, 0.043008, 0.073780, 0.035838, 0.080650, 0.071433, 0.073493, 0.026725,
    0.002181, 0.002203, 0.112864, 0.060140, 0.115635, 0.065531, 0.093277, 0.094123, 0.093125, 0.144290,
    0.002397, 0.002369, 0.043241, 0.002518, 0.040455, 0.002656, 0.002540, 0.090915, 0.002443, 0.101604,
    0.002598, 0.002547, 0.002748, 0.002939, 0.002599, 0.003395, 0.002733, 0.003774, 0.002659, 0.004583,
    0.003277, 0.003176, 0.003265, 0.004301, 0.003160, 0.004517, 0.003833, 0.008354, 0.003140, 0.009214,
    0.008558, 0.007646, 0.007622, 0.026437, 0.007633, 0.021560, 0.007622, 0.017570, 0.007632, 0.018037,
    0.031062, 0.028428, 0.028428, 0.108300, 0.028751, 0.111013, 0.028428, 0.048661, 0.028699, 0.061490,
    0.051063, 0.047597, 0.048824, 0.129541, 0.045247, 0.124975, 0.047804, 0.128904, 0.045053, 0.119087,
    0.002197, 0.002552, 0.002098, 0.200688, 0.002073, 0.102060, 0.002111, 0.163116, 0.002125, 0.165419,
    0.002060, 0.002504, 0.002105, 0.166820, 0.002117, 0.144274, 0.005074, 0.143881, 0.004875, 0.205333,
    0.001852, 0.002184, 0.002167, 0.163804, 0.002132, 0.212644, 0.003431, 0.244546, 0.004205, 0.315848,
    0.002450, 0.002360, 0.002243, 0.154635, 0.002246, 0.148259, 0.002239, 0.348694, 0.002265, 0.368426,
    0.002321, 0.002393, 0.002376, 0.074124, 0.002439, 0.126918, 0.002453, 0.439270, 0.002416, 0.489812,
    0.002484, 0.002629, 0.002559, 0.150246, 0.002579, 0.140103, 0.002548, 0.493103, 0.002637, 0.509481,
    0.002960, 0.002952, 0.002880, 0.294884, 0.002758, 0.332805, 0.002727, 0.455842, 0.002816, 0.431807,
    0.003099, 0.003028, 0.002927, 0.387154, 0.002899, 0.397946, 0.002957, 0.261333, 0.002909, 0.148548,
    0.004887, 0.004884, 0.006581, 0.414647, 0.003735, 0.431317, 0.006426, 0.148997, 0.003736, 0.080715,
    0.001969, 0.002159, 0.002325, 0.200211, 0.002288, 0.202137, 0.002289, 0.595331, 0.002311, 0.636097
};

cbuffer GlobalCSBuffer : register(b8)
{
    float3 g_TotalVel;
    float g_FrameRate;
}

struct VertexRain
{
    float3 Position : POSITION0;
    float3 Seed : SEED0;
    float3 Speed : SPEED0;
    float Random : RANDOM0;
    uint Type : TYPE0;
};

///////////////////////////////////////////////////////////////////////////////
//VS part

VertexRain VS(VertexRain input)
{
    VertexRain output = input;

    //move forward
    output.Position.xyz = input.Position.xyz + (input.Speed.xyz / g_FrameRate + g_TotalVel.xyz);
    //output.Position.xyz += output.Speed.xyz / g_FrameRate + g_TotalVel.xyz;
    float3 cPos = CameraPosition();
    
    //float x = output.Seed.x + cPos.x;
    //float z = output.Seed.z + cPos.z;
    //float y = output.Seed.y + cPos.y;
    
    //if the particle is outside the bounds, move it to random position near the eye         
    //output.Position.xyz = (output.Position.y <= cPos.y - g_heightRange) ? float3(x, y, z) : output.Position.xyz;
    [branch]
    if (output.Position.y <= cPos.y - g_heightRange)
    {
        float x = output.Seed.x + cPos.x;
        float z = output.Seed.z + cPos.z;
        float y = output.Seed.y + cPos.y;
        output.Position.xyz = float3(x, y, z);
    }
    //output.Speed = output.Position.xyz;

    return output;
}

VertexRain VSPassThrough(VertexRain input)
{
    return input;
}

///////////////////////////////////////////////////////////////////////////////
//GS part

cbuffer GS_Light : register(b7) // 버텍스 쉐이더랑 별개이므로 다시 0번부터
{
    float3 GSDirection;
    float GS_Light_Padding;
    
    float3 GSPosition;
    float GS_Light_Padding2;
}

struct PSSceneIn
{
    float4 Position : SV_POSITION;
    float3 LightDir : LIGHTDIR0;
    float3 PointLightDir : POINTLIGHTDIR0;
    float3 EyeVec : EYEVEC0;
    float2 Uv : UV0;
    uint Type : TYPE0;
    float Random : RANDOM0;
    float3 Speed : SPEED0;
};


void GenRainSpriteVertices(float3 worldPos, float3 velVec, float3 eyePos, out float3 outPos[4])
{
    float height = g_SpriteSize / 2.0;
    float width = height / 10.0;

    velVec = normalize(velVec);
    float3 eyeVec = eyePos - worldPos;
    float3 eyeOnVelVecPlane = eyePos - ((dot(eyeVec, velVec)) * velVec);
    float3 projectedEyeVec = eyeOnVelVecPlane - worldPos;
    float3 sideVec = normalize(cross(projectedEyeVec, velVec));
    
    outPos[0] = worldPos - (sideVec * 0.5 * width);
    outPos[1] = outPos[0] + (sideVec * width);
    outPos[2] = outPos[0] + (velVec * height);
    outPos[3] = outPos[2] + (sideVec * width);
}

bool cullSprite(float3 position, float SpriteSize)
{
    float4 vpos;
    vpos = mul(float4(position, 1), World);
    vpos = mul(vpos, View);
    
    [branch]
    if ((vpos.z < (g_Near - SpriteSize)) || (vpos.z > (g_Far + SpriteSize)))
    {
        return true;
    }
    else
    {
        float4 ppos = mul(vpos, Projection);
        float wext = ppos.w + SpriteSize;

        [branch]
        if ((ppos.x < -wext) || (ppos.x > wext) ||
            (ppos.y < -wext) || (ppos.y > wext))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    
    return false;
}

//GeometryShader gsStreamOut = ConstructGSWithSO(CompileShader(vs_5_0, VSAdvanceRain()), "POSITION.xyz; SEED.xyz; SPEED.xyz; RAND.x; TYPE.x;");
[maxvertexcount(1)]
void gsStreamOut(point VertexRain input[1], inout PointStream<VertexRain> SpriteStream)
{
    SpriteStream.Append(input[0]);
}

// GS for rendering rain as point sprites.  Takes a point and turns it into 2 tris.
[maxvertexcount(4)]
void GS(point VertexRain input[1], inout TriangleStream<PSSceneIn> SpriteStream)
{
    float totalIntensity = g_PointLightIntensity * g_ResponsePointLight + dirLightIntensity * g_ResponseDirLight;

    if (!cullSprite(input[0].Position, 2.0f * g_SpriteSize) && totalIntensity > 0)
    {
        //PSSceneIn output;
    
        //float3 up = float3(0, 1, 0);
        //float3 look = input[0].Position.xyz - CameraPosition();
        //look.y = 0.0f; // 수직고정
        //look = normalize(look);

        //float3 right = cross(up, look);
    
        //float halfWidth = 1.0f * 0.5f;
        //float halfHeight = 1.0f * 0.5f;

        //float4 v[4];
        //v[0] = float4(input[0].Position.xyz - halfWidth * right - halfHeight * up, 1.0f);
        //v[1] = float4(input[0].Position.xyz + halfWidth * right - halfHeight * up, 1.0f);
        //v[1] = float4(input[0].Position.xyz + halfHeight * up, 1.0f);
        //v[2] = float4(input[0].Position.xyz + halfWidth * right - halfHeight * up, 1.0f);
        ////v[0] = float4(input[0].Position.xyz - float3(halfWidth, halfHeight, 0), 1.0f);
        ////v[1] = float4(input[0].Position.xyz + float3(0, halfHeight, 0), 1.0f);
        ////v[2] = float4(input[0].Position.xyz - float3(-halfWidth, halfHeight, 0), 1.0f);

    
        //output.EyeVec = float3(0, 0, 0);
        //output.LightDir = float3(0, 0, 0);
        //output.PointLightDir = float3(0, 0, 0);
        //output.Random = input[0].Random;
        //output.Type = 0;
        //output.Uv = float2(0, 0);
        //output.Speed = input[0].Speed;

        //[unroll]
        //for (int i = 0; i < 3; ++i)
        //{
        //    output.Position = mul(v[i], View);
        //    output.Position = mul(output.Position, Projection);

        //    output.LightDir = float3(1, 0, 1);
        //    SpriteStream.Append(output);
        //}
        //SpriteStream.RestartStrip();
    
        PSSceneIn output = (PSSceneIn) 0;
        output.Type = input[0].Type;
        output.Random = input[0].Random;
       
        float3 pos[4];
        float3 camPos = CameraPosition();
        GenRainSpriteVertices(input[0].Position.xyz, input[0].Speed.xyz / g_FrameRate + g_TotalVel, camPos, pos);
    
        //output.EyeVec = float3(0, 0, 0);
        //output.LightDir = float3(0, 0, 0);
        //output.PointLightDir = float3(0, 0, 0);
        //output.Random = input[0].Random;
        //output.Type = 0;
        //output.Uv = float2(0, 0);
        //output.Speed = input[0].Speed;
    
        //output.Position = mul(float4(pos[0], 1.0), View);
        //output.Position = mul(output.Position, Projection);
        //output.Uv = g_texcoords[0];
        //SpriteStream.Append(output);
    
        //output.Position = mul(float4(pos[1], 1.0), View);
        //output.Position = mul(output.Position, Projection);
        //output.Uv = g_texcoords[1];
        //SpriteStream.Append(output);
    
        //output.Position = mul(float4(pos[2], 1.0), View);
        //output.Position = mul(output.Position, Projection);
        //output.Uv = g_texcoords[2];
        //SpriteStream.Append(output);
    
        //output.Position = mul(float4(pos[3], 1.0), View);
        //output.Position = mul(output.Position, Projection);
        //output.Uv = g_texcoords[3];
        //SpriteStream.Append(output);

        //SpriteStream.RestartStrip();

    
        
        float3 closestPointLight = g_PointLightPos;
        float closestDistance = length(g_PointLightPos - pos[0]);
        if (length(g_PointLightPos2 - pos[0]) < closestDistance)
            closestPointLight = g_PointLightPos2;
        
        output.Position = mul(float4(pos[0], 1.0), View);
        output.Position = mul(output.Position, Projection);
        output.LightDir = GSDirection - pos[0];
        output.PointLightDir = closestPointLight - pos[0];
        output.EyeVec= camPos - pos[0];
        output.Uv = g_texcoords[0];
        SpriteStream.Append(output);
                
        output.Position = mul(float4(pos[1], 1.0), View);
        output.Position = mul(output.Position, Projection);
        output.LightDir = GSDirection - pos[1];
        output.PointLightDir = closestPointLight - pos[1];
        output.EyeVec = camPos - pos[1];
        output.Uv = g_texcoords[1];
        SpriteStream.Append(output);
        
        output.Position = mul(float4(pos[2], 1.0), View);
        output.Position = mul(output.Position, Projection);
        output.LightDir = GSDirection - pos[2];
        output.PointLightDir = closestPointLight - pos[2];
        output.EyeVec = camPos - pos[2];
        output.Uv = g_texcoords[2];
        SpriteStream.Append(output);
                
        output.Position = mul(float4(pos[3], 1.0), View);
        output.Position = mul(output.Position, Projection);
        output.LightDir = GSDirection - pos[3];
        output.PointLightDir = closestPointLight - pos[3];
        output.EyeVec = camPos - pos[3];
        output.Uv = g_texcoords[3];
        SpriteStream.Append(output);
        
        SpriteStream.RestartStrip();
    }
}

///////////////////////////////////////////////////////////////////////////////
//PS part

Texture2DArray rainTextureArray : register(t10);
SamplerState samAniso
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PS(PSSceneIn input) : SV_TARGET
{
    float4 color;
    color = rainTextureArray.Sample(samAniso, float3(input.Uv, 1));
    return color;
}
