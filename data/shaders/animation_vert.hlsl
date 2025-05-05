static const int MAX_BONES = 100;
static const int MAX_BONE_INFLUENCE = 4;

struct VS_Input
{
    float3 pos : POSITION;
    float3 nor : NORMAL;
    float2 uv  : TEXCOORD0;
    int4 boneIds   : TEXCOORD1;
    float4 weigths : TEXCOORD2;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float3 nor : NORMAL;
    float2 uv : TEXCOORD0;
    float3 fragPos : TEXCOORD1;
};

cbuffer Matrices : register(b0)
{
    float4x4 view;
    float4x4 proj;
};

cbuffer PerDrawCall : register(b1)
{
    float4x4 model;
};

cbuffer Animation : register(b2)
{
    matrix boneMatrix[MAX_BONES];
}

PS_Input vs_main(VS_Input i)
{
    PS_Input o = (PS_Input)0;

    float4 totalPosition = float4(0, 0, 0, 0);

    for(int j = 0; j < MAX_BONE_INFLUENCE; ++j) {
        if(i.boneIds[j] == -1) {
            continue;
        }
        
        if(i.boneIds[j] >= MAX_BONES) {
            totalPosition = float4(i.pos, 1.0f);
            break;
        }

        float4 localPosition = mul(boneMatrix[i.boneIds[j]], float4(i.pos, 1.0f));
        totalPosition += localPosition * i.weigths[j];
    }

    float4 wPos = mul(model, totalPosition);
    float3 fragPos = float3(wPos.xyz);
    wPos = mul(view, wPos);
    wPos = mul(proj, wPos);

    float3 wNor = mul((float3x3) model, i.nor);
    wNor = normalize(wNor);

    o.pos = wPos;
    o.nor = wNor;
    o.uv = i.uv;
    o.fragPos = fragPos;
    return o;
}