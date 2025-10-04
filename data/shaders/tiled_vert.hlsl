#pragma pack_matrix(row_major) 

struct VS_Input
{
    float3 pos : POSITION;
    float3 nor : NORMAL;
    float3 tan : TEXCOORD0;
    float3 bit : TEXCOORD1;
    float2 uv : TEXCOORD2;
    int4 boneIds : TEXCOORD3;
    float4 weigths : TEXCOORD4;
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
    float width;
    float height;
};

PS_Input vs_main(VS_Input i)
{
    PS_Input o = (PS_Input)0;
    float4 wPos = mul(float4(i.pos, 1.0f), model);
    float3 fragPos = float3(wPos.xyz);
    wPos = mul(wPos, view);
    wPos = mul(wPos, proj);

    float3 wNor = mul(i.nor, (float3x3)model);
    wNor = normalize(wNor);

    float2 uv = i.uv;
    uv.x *= width;
    uv.y *= height;
    
    o.pos = wPos;
    o.nor = wNor;
    o.uv = uv;
    o.fragPos = fragPos;
    return o;
}