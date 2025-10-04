struct PS_Input
{
    float4 pos : SV_POSITION;
    float3 nor : NORMAL;
    float2 uv : TEXCOORD0;
    float3 fragPos : TEXCOORD1;
    float3x3 TBN : TEXCOORD2;
};

cbuffer SolidColorUbo : register(b0)
{
    float3 ambient;
    float3 diffuse;
    float3 specular;
    float shininess;
};

float4 fs_main(PS_Input i) : SV_TARGET
{
    float3 color = ambient;
    return float4(color, 1.0f);
}