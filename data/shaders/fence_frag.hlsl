SamplerState samplerState : register(s0);

Texture2D diffuseMap : register(t0);
Texture2D noiseMap : register(t1);

cbuffer PortalUbo : register(b0)
{
    float time;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float3 nor : NORMAL;
    float2 uv : TEXCOORD0;
    float3 fragPos : TEXCOORD1;
};

float4 fs_main(PS_Input i) : SV_TARGET
{
    float4 color = diffuseMap.SampleLevel(samplerState, i.uv, 0);
    if (color.a < 0.5f)
    {
        discard;
    }
    float3 finalColor = color.rgb * float3(0.75f, 0.75f, 0.75f);
    return float4(finalColor, 1.0f);
}