#pragma once

#include "../Inc/DX11Define.h"

// 2023 07 06 이정모 home

// 파서가 파싱을 완료하면 mesh object에 대한 데이터가 전부 로드되며
// 해당 데이터를 이 class에 가져와서 관련 자원을 생성하고
// resource manager가 이 자원들을 관리하게됨

// 네임스페이스로 감싸진 struct의 전방 선언
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