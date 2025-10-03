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

struct DirectionalLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float3 direction;
    float pad;
};

struct PointLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float3 position;
    float range;
    float3 att;
    float pad;
};

cbuffer LightConstBuffer : register(b4)
{
    DirectionalLight directionalLight;
    PointLight pointLights[8];
    int lightCount;
    float3 viewPos;
};


void CalcPointLight(float4 color, PointLight L, float3 pos,
    float3 normal, float3 toEye,
    out float4 ambient,
    out float4 diffuse,
    out float4 spec)
{
    // Initialize outputs
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 lightVec = L.position - pos;
    float d = length(lightVec);
    
    if(d > L.range)
    {
        return;
    }
    
    lightVec /= d;
    
    // Ambient term
    ambient = color * L.ambient;
    
    float diffuseFactor = dot(lightVec, normal);
    
    [flatten]
    if(diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), 4);
        
        diffuse = diffuseFactor * color * L.diffuse;
        spec = specFactor * float4(1, 1, 1, 1) * L.specular;
    }
}

void CalcDirectionalLight(float4 color, DirectionalLight L,
    float3 normal, float3 toEye, 
    out float4 ambient,
    out float4 diffuse,
    out float4 spec)
{
    // Initialize outputs
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // The light vector aims opposite the direction the light rays travel
    float3 lightVec = -L.direction;
    
    // Add ambient term
    ambient = color * L.ambient;
    
    // Add diffuse and specular term, provided the surface is in
    // the line of site of the light
    float diffuseFactor = dot(lightVec, normal);
    
    // Flatten to avoid dynamic branching
    [flatten]
    if(diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), 8);
        
        diffuse = diffuseFactor * color * L.diffuse;
        spec = specFactor * float4(1, 1, 1, 1) * L.specular;
    }
}


float4 fs_main(PS_Input i) : SV_TARGET
{
    float4 baseColor = diffuseMap.Sample(samplerState, i.uv);
    float3 n = normalize(i.nor);    
    float3 toEye = normalize(viewPos - i.fragPos);
    
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 A, D, S;
    
    CalcDirectionalLight(baseColor, directionalLight, n, toEye, A, D, S);
    ambient += A;
    diffuse += D;
    spec    += S;
    
    for (int index = 0; index < 8; index++)
    {
        CalcPointLight(baseColor, pointLights[index], i.fragPos, n, toEye, A, D, S);
        ambient += A;
        diffuse += D;
        spec    += S;
    }
    
    return ambient + diffuse + spec;
}