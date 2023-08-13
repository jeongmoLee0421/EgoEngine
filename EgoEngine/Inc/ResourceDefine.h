#pragma once

// 2023 06 16 이정모 home

// 자원들에 대한 define

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

// 어차피 index buffer size는 IB를 생성할 때 정해져 있기 때문에
// 굳이 enum class를 따로 빼서 관리할 필요 없이 위의 EResourceType 사용하기로 결정
// 오브젝트가 생길 때마다 enum을 두개씩 추가하지 않아도 됨
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