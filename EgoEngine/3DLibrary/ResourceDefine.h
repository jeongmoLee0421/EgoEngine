#pragma once

// 2023 06 16 ������ home

// �ڿ��鿡 ���� define

enum class EResourceType
{
	None,

	Device,
	DeviceContext,

	VBGrid,
	VBWorldAxis,
	VBTextureBox,
	VBBoxMaterial,

	IBGrid,
	IBWorldAxis,
	IBTextureBox,
	IBBoxMaterial,

	CBWorldViewProj,

	VSColor,
	VSTexture,
	VSNormalTexture,
	
	ILColor,
	ILTexture,
	ILNormalTexture,

	PSColor,
	PSTexture,
	PSNormalTexture,

	SamplerLinear,

	TexDarkBrick,
	TexWoodCrate01,

	Max,
};

// ������ index buffer size�� IB�� ������ �� ������ �ֱ� ������
// ���� enum class�� ���� ���� ������ �ʿ� ���� ���� EResourceType ����ϱ�� ����
// ������Ʈ�� ���� ������ enum�� �ΰ��� �߰����� �ʾƵ� ��
//enum class EIndexBufferSizeType
//{
//	None,
//
//	Grid,
//	WorldAxis,
//	TextureBox,
//
//	Max,
//};