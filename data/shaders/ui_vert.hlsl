struct VS_Input
{
    float2 pos : POSITIONT;
    float2 uv : TEXCOORD0;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

cbuffer Matrices : register(b0)
{
    float4x4 view;
    float4x4 proj;
    float4x4 model;
};

PS_Input vs_main(VS_Input i)
{    
    PS_Input o = (PS_Input) 0;
    float4 wPos = mul(float4(i.pos, 0.0f, 1.0f), model);
    wPos = mul(wPos, view);
    wPos = mul(wPos, proj);

    o.pos = wPos;
    o.uv = i.uv;
    
    return o;
}