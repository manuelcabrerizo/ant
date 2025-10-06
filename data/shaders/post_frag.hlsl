SamplerState samplerStateWrap : register(s0);
SamplerState samplerStateClamp : register(s1);

Texture2D frameMap : register(t0);
Texture2D weaponFrameMap : register(t1);
Texture2D bloomMap : register(t2);

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 fs_main(PS_Input i) : SV_TARGET
{    
    const float exposure = 1.0f;
    const float3 one = float3(1.0f, 1.0f, 1.0f);
    const float gamma = 2.2f;
    const float oneOverGama = 1.0f / gamma;
    
    float3 hdrColor = frameMap.Sample(samplerStateClamp, i.uv).rgb;
    float3 weaponHdrColor = weaponFrameMap.Sample(samplerStateClamp, i.uv).rgb;
    float3 bloomColor = bloomMap.Sample(samplerStateClamp, i.uv).rgb;
    
    float mask = step(0.00001f, length(weaponHdrColor));   
    hdrColor = lerp(hdrColor, weaponHdrColor, mask); 
        
    hdrColor += bloomColor;
    
    // reinhard tone mapping
    float3 mapped =  one - exp(-hdrColor * exposure);
    // gamma correction 
    mapped = pow(abs(mapped), float3(oneOverGama, oneOverGama, oneOverGama));
    
    
    
    return float4(mapped, 1.0f);
}