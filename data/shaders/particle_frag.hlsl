SamplerState samplerState : register(s0);
Texture2D diffuseMap : register(t0);

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 col : TEXCOORD1;
};

float4 fs_main(PS_Input i) : SV_TARGET
{
    float4 color = diffuseMap.Sample(samplerState, i.uv);
    color.rgb *= i.col;
    /*
    if (color.a < 0.5f)
    {
        discard;
    }
    */
    return color;
}