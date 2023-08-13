// 2023 06 28 이정모 home

// texture 좌표를 받아서 표면에 입혀줌

// 일반적인 RGB값이 들어있는 텍스처
// 숫자가 아닌 것들은 상수 버퍼에 넣을 수 없다고 한다.
Texture2D texDiffuseMap: register(t0);
SamplerState samLinear: register(s0);

cbuffer ConstantBuffer: register(b0)
{
	float4x4 WorldViewProj;
};

struct VS_INPUT
{
	float3 Pos: POSITION;
	float2 Tex: TEXCOORD;
};

struct PS_INPUT
{
	float4 Pos: SV_POSITION;
	float2 Tex: TEXCOORD;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	output.Pos = mul(float4(input.Pos, 1.f), WorldViewProj);
	output.Tex = input.Tex;

	return output;
}