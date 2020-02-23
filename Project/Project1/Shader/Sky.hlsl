#include "Common.hlsl"

struct VertexOut
{
	float4 svPosition       : SV_POSITION;
	float3 localPosition    : POSITION;
};

TextureCube cubeMap : register(t0);
SamplerState LinearSample : register(s0);


VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    
    vout.localPosition = vin.position;
    
    float4 shpereCenter = mul(model, float4(vin.position, 1.0f));
    shpereCenter.xyz += cameraPos;
    
    float4x4 viewProj = mul(project, view);
    vout.svPosition = mul(viewProj, shpereCenter).xyww;
    
    return vout;
}

float4 PS(VertexOut vin) : SV_Target
{
    return cubeMap.Sample(LinearSample, vin.localPosition).bgra;
}

