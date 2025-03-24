struct PS_Input
{
    float4 pos : SV_POSITION;
    float3 col : COLOR;
};

float4 fs_main(PS_Input i) : SV_TARGET
{
    return float4(i.col, 1.0f);
}