// 2023/06/10 ������ home

// WorldViewProj�� ���� ī�޶� ��ǥ��� ��ȯ�ѵ�
// ������ ���� ���

// �����ϵ� ���̴� ������Ʈ�� �о ���̴��� ����� ������ vs, ps�� ��������

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