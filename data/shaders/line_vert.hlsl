#pragma pack_matrix(row_major) 

struct VS_Input
{
    float3 pos : POSITION;
    float3 col : COLOR;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float3 col : COLOR;
};

cbuffer Matrices : register(b0)
{
    float4x4 view;
    float4x4 proj;
};

PS_Input vs_main(VS_Input i)
{
    PS_Input o = (PS_Input)0;

    float4 wPos = float4(i.pos, 1.0f);
    wPos = mul(wPos, view);
    wPos = mul(wPos, proj);

    o.pos = wPos;
    o.col = i.col;
    return o;
}