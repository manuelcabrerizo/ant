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
    float2 noiseUv = i.uv * 0.1f;
    float2 noise0 = noiseMap.Sample(samplerState, float2(noiseUv.x + time*0.01f, noiseUv.y)).xy;
    float2 noise1 = noiseMap.Sample(samplerState, float2(-noiseUv.x + time * 0.01f, noiseUv.y + time * 0.01f)).xy;
    float2 noise = (noise0 + noise1);    
    float2 textureUv = i.uv + (noise * 0.125f);
    float4 color = diffuseMap.Sample(samplerState, textureUv);
    color *= 4.0f;
    return color;
}