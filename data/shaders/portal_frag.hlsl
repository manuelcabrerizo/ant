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
    float2 noiseUv = i.uv;
    noiseUv.y += time*0.1f;
    float4 noise = noiseMap.Sample(samplerState, noiseUv*0.25f);

    float2 textureUv = i.uv + noise.xy;
    float4 color = diffuseMap.Sample(samplerState, textureUv);
    color *= 4.0f;
    
    return color;
}