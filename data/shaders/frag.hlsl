SamplerState samplerState : register(s0);

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D spacularMap : register(t2);

struct PS_Input
{
    float4 pos : SV_POSITION;
    float3 nor : NORMAL;
    float2 uv : TEXCOORD0;
    float3 fragPos : TEXCOORD1;
};

cbuffer TextureUbo : register(b0)
{
    float shininess;
};

float4 fs_main(PS_Input i) : SV_TARGET
{
    float3 ligthPos0 = float3(30.0f, 20.0f, 20.0f);
    float3 ligthDir0 = normalize(ligthPos0 - i.fragPos);
    float3 lightColor0 = float3(0.4f, 0.4f, 0.8f);

    float3 ligthPos1 = float3(0.0f, 20.0f, -20.0f);
    float3 ligthDir1 = normalize(ligthPos1 - i.fragPos);
    float3 lightColor1 = float3(0.4f, 0.4f, 0.8f);
    
    float3 ligthPos2 = float3(0.0f, 20.0f, 40.0f);
    float3 ligthDir2 = normalize(ligthPos2 - i.fragPos);
    float3 lightColor2 = float3(0.4f, 0.8f, 0.4f);

    float3 ligthPos3 = float3(-30.0f, 20.0f, 20.0f);
    float3 ligthDir3 = normalize(ligthPos3 - i.fragPos);
    float3 lightColor3 = float3(0.8f, 0.4f, 0.4f);

    float3 color = diffuseMap.Sample(samplerState, i.uv).rgb;

    float3 n = normalize(i.nor);
    float diffuse0 = max(dot(ligthDir0, n), 0.1f);
    float diffuse1 = max(dot(ligthDir1, n), 0.1f);
    float diffuse2 = max(dot(ligthDir2, n), 0.1f);
    float diffuse3 = max(dot(ligthDir3, n), 0.1f);

    float3 finalColor = color * ((lightColor0 * diffuse0) + (lightColor1 * diffuse1) + (lightColor2 * diffuse2) + (lightColor3 * diffuse3));

    return float4(finalColor, 1.0f);
}