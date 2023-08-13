#pragma once

#include "../Inc/DX11Define.h"

// 2023 07 06 ������ home

// �ļ��� �Ľ��� �Ϸ��ϸ� mesh object�� ���� �����Ͱ� ���� �ε�Ǹ�
// �ش� �����͸� �� class�� �����ͼ� ���� �ڿ��� �����ϰ�
// resource manager�� �� �ڿ����� �����ϰԵ�

// ���ӽ����̽��� ������ struct�� ���� ����
namespace ASE
{
	struct Mesh;
}

class ResourceManager;

class ObjectResourceLoadingClass
{
public:
	ObjectResourceLoadingClass();
	~ObjectResourceLoadingClass();

public:
	void LoadGeomerty(ASE::Mesh* pMesh, ID3D11Device* pDevice, ResourceManager* pResourceManager);

private:
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	unsigned int mIndexBufferSize;
};