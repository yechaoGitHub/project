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
	float3 N = normalize(vin.localPosition);
	float3 irradiance = float3(0, 0, 0);

	float3 up = float3(0, 1, 0);
	float3 right = cross(up, N);
	up = cross(N, right);

	float sampleDelta = 0.025;
	float nrSamples = 0.0;

	for (float phi = 0.0; phi < 2 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
			irradiance += cubeMap.Sample(LinearSample, sampleVec).bgr * cos(theta) * sin(theta);
			nrSamples++;
		}
	}

	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	return float4(irradiance, 1.0);
}
