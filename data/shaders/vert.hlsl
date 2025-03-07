struct VS_Input
{
    float3 pos : POSITION;
    float3 nor : NORMAL;
    float2 uv  : TEXCOORD0;
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

PS_Input vs_main(VS_Input i)
{
    PS_Input o = (PS_Input)0;

    float4 wPos = mul(model, float4(i.pos, 1.0f));
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