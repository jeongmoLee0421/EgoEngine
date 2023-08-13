// 2023 07 13 이정모 home

// normal, texture를 사용하는 셰이더

Texture2D texDiffuseMap: register(t0);
SamplerState samLinear: register(s0);

cbuffer ConstantBuffer: register(b0)
{
	float4x4 WorldViewProj;
};

struct VS_INPUT
{
	float3 Pos: POSITION;
	float3 Normal: NORMAL;
	float2 Tex: TEXCOORD;
};

struct PS_INPUT
{
	float4 Pos: SV_POSITION;
	float3 Normal: NORMAL;
	float2 Tex: TEXCOORD;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	output.Pos = mul(float4(input.Pos, 1.f), WorldViewProj);
	output.Normal = input.Normal;
	output.Tex = input.Tex;

	return output;
}