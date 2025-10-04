SamplerState samplerStateWrap : register(s0);
SamplerState samplerStateClamp : register(s1);



Texture2D diffuseMap : register(t0);

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 fs_main(PS_Input i) : SV_TARGET
{
    float3 hdrColor = diffuseMap.Sample(samplerStateClamp, i.uv).rgb;
    float4 result = float4(0.0, 0.0, 0.0, 0.0);
    float brightness = dot(hdrColor.rgb, float3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
        result = float4(hdrColor.rgb, 1.0);
    else
        result = float4(0.0, 0.0, 0.0, 1.0);
    
    return result;
}