#include "ObjectResourceLoadingClass.h"
#include "../Inc/ASEParsingData.h"
#include "../Inc/ShaderDefine.h"
#include "../Inc/ResourceManager.h"

#include <vector>

ObjectResourceLoadingClass::ObjectResourceLoadingClass()
	: mVertexBuffer(nullptr)
	, mIndexBuffer(nullptr)
	, mIndexBufferSize(0)
{
}

ObjectResourceLoadingClass::~ObjectResourceLoadingClass()
{
}

// 정점 최적화가 모두 끝난 뒤의 정점과 인덱스를 가지고 buffer를 생성하는 단계
void ObjectResourceLoadingClass::LoadGeomerty(ASE::Mesh* pMesh, ID3D11Device* pDevice, ResourceManager* pResourceManager)
{
	std::vector<PosNormalTexVertex> vertices(pMesh->mOptimizedVertexList.size());
	for (int i = 0; i < vertices.size(); ++i)
	{
		vertices[i].Position = pMesh->mOptimizedVertexList[i]->mPosition;
		vertices[i].Normal = pMesh->mOptimizedVertexList[i]->mNormal;
		vertices[i].Texture = pMesh->mOptimizedVertexList[i]->mTextureCoordinate;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = static_cast<UINT>(sizeof(PosNormalTexVertex) * vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = &vertices[0];
	HR(pDevice->CreateBuffer(&bd, &initData, &mVertexBuffer));
	pResourceManager->RegisterResource<ID3D11Buffer>(pMesh->mVBResourceType, mVertexBuffer);
	mVertexBuffer->Release(); // Resource Manager 내부에서 IResource가 생성되면 AddRef()가 생성자에서 불림

	std::vector<UINT> indices;
	for (const auto& vertexIndex : pMesh->mOptimizedIndicesList)
	{
		indices.push_back(vertexIndex);
	}

	mIndexBufferSize = static_cast<unsigned int>(indices.size());
	pResourceManager->RegisterIndexBufferSize(pMesh->mIBSizeType, mIndexBufferSize);

	bd.Usage = D3D11_USAGE_DEFAULT;
	// index 하나의 크기를 UINT(4byte)로 했기 때문에 WORD(2byte)로 계산하면 안됨
	bd.ByteWidth = sizeof(UINT) * mIndexBufferSize;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	initData.pSysMem = &indices[0];
	HR(pDevice->CreateBuffer(&bd, &initData, &mIndexBuffer));
	pResourceManager->RegisterResource<ID3D11Buffer>(pMesh->mIBResourceType, mIndexBuffer);
	mIndexBuffer->Release();
}
