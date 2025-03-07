Texture2D srv : register(t0);
SamplerState samplerState : register(s0);

struct PS_Input
{
    float4 pos : SV_POSITION;
    float3 nor : NORMAL;
    float2 uv : TEXCOORD0;
    float3 fragPos : TEXCOORD1;
};

float4 fs_main(PS_Input i) : SV_TARGET
{
    float3 ligthPos = float3(2.0f, 1.0f, 1);
    float3 ligthDir = normalize(ligthPos - i.fragPos);
    float3 color = srv.Sample(samplerState, i.uv).rgb;

    float3 n = normalize(i.nor);
    float diffuse = max(dot(ligthDir, n), 0.2f);

    return float4(color * diffuse, 1.0f);
}