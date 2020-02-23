cbuffer ObjectProperty : register(b0)
{
	float4x4 modelViewProj;
	float4x4 model;
	float4x4 view;
	float4x4 project;
};

cbuffer GlobalInfo : register(b1)
{
	float3 cameraPos;
	float3 cameraDirection;
};

struct VertexIn
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float3 tangant  : TANGENT;
	float2 texCoord : TEXCOORD;
};

static const float PI = 3.14159265359;