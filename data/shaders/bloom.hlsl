SamplerState samplerStateWrap : register(s0);
SamplerState samplerStateClamp : register(s1);

Texture2D diffuseMap : register(t0);

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

cbuffer bloomUbo : register(b5)
{
    int horizontal;
};

float4 fs_main(PS_Input i) : SV_TARGET
{
    float weight[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };

    uint width, height, levels;
    diffuseMap.GetDimensions(0, width, height, levels);
    float2 texOffset = 1.0f / float2((float)width, (float)height);
    float3 result = diffuseMap.Sample(samplerStateClamp, i.uv).rgb * weight[0];
    if (horizontal > 0.0)
    {
        for (int index = 1; index < 5; ++index)
        {
            result += diffuseMap.Sample(samplerStateClamp, i.uv + float2(texOffset.x * index, 0.0)).rgb * weight[index];
            result += diffuseMap.Sample(samplerStateClamp, i.uv - float2(texOffset.x * index, 0.0)).rgb * weight[index];
        }
    }
    else
    {
        for (int index = 1; index < 5; ++index)
        {
            result += diffuseMap.Sample(samplerStateClamp, i.uv + float2(0.0, texOffset.y * index)).rgb * weight[index];
            result += diffuseMap.Sample(samplerStateClamp, i.uv - float2(0.0, texOffset.y * index)).rgb * weight[index];
        }
    }

    return float4(result, 1.0f);    
}