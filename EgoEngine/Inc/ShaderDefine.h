#pragma once

#include "../Inc/DX11Define.h"

// 2023 06 13 ������ home

// factory, builder, resourcemanager�� ����鼭
// ���� ������ �����ϴ� shader define�� �����

struct ColorVertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
};

struct TextureVertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TextureCoordinate;
};

struct NormalVertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
};

struct PosNormalTexVertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 Texture;
};

struct ConstantBufferWVP
{
	DirectX::XMMATRIX WorldViewProj;
};