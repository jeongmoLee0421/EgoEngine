// 2023/06/10 이정모 home

// WorldViewProj을 통해 카메라 좌표계로 변환한뒤
// 지정한 색을 출력

// 컴파일된 셰이더 오브젝트를 읽어서 셰이더를 만들기 때문에 vs, ps가 나뉘었음

cbuffer ConstantBuffer
{
	float4x4 WorldViewProj;
};

struct VS_INPUT
{
	float3 Pos: POSITION;
	float4 Color: COLOR;
};

struct PS_INPUT
{
	float4 Pos: SV_POSITION;
	float4 Color: COLOR;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	output.Pos = mul(float4(input.Pos, 1.f), WorldViewProj);
	output.Color = input.Color;

	return output;
}