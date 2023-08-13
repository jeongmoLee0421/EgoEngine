#pragma once

#include "../Inc/DX11Define.h"
#include "../Inc/ResourceDefine.h"
#include "../Inc/ParentResource.h"
#include "../Inc/IResource.h"
#include "../Inc/ChildResource.h"
#include "../Inc/ConceptDefine.h"

#include <wrl/client.h>
#include <unordered_map>
#include <cassert>

// 2023 06 13 ������ home

// ���������� ����ϴ�
// vertex buffer, index buffer, constant buffer,
// vertex shader, input layout, pixel shader,
// texture ���� �ε��ϰ� �����ϴ� �Ŵ��� Ŭ����

// ��ü ������ ���� ������ ������ �� �ڵ��� �Ѱ���
// ��ü���� �ʿ��� ���� �ڵ��� ������� �ڿ��� ���� �����͸� ���ͼ� ���

// �ڿ� ���� ���
// 1. ���� ���� �� handle�� Set �ϴ� �������� AddRef()
// ���� �Ҹ� �� �Ҹ��ڿ��� Release()

// 2. ������ ������� ���丮�� ������Ʈ�� �����ϸ鼭 �����ڷ� �ڵ��� �Ѿ��
// Initialize()���� �ش� �ڵ��� ����ϴ� �ڵ��̶�� AddRef()
// ��ü �Ҹ� �� Finalize()���� ����ϴ� �ڵ鿡 ���ؼ��� Release()

// 3. ��� �ڿ��� ���� ������ �����ǰ�
// ���������� ���ҽ� �Ŵ����� �ڵ�-���ҽ� ���� ��ȸ�ϸ鼭 ��� �ڿ��� ���� Release() ȣ��

class ASEParser;
class ObjectResourceLoadingClass;

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	void Initialize(void* pDevice, void* pDeviceContext);
	void Finalize();

public:
	// �ڿ��� ���� �ڵ��� ��û�ϴ� ���� ��ü�� ����ִ� ���ȿ��� �������� �ڿ��� ã�� �� �ֱ� ������ AddRef()
	unsigned int GetHandleResource(EResourceType eResourceType);

	// �ڿ��� ���� �����͸� ��û�� ��
	// ���� ��������� ��� ��� �����Ŷ�� �������� �ڿ��� ������ �� �ֱ� ������ AddRef()�� �ϰ� �Ҹ��� �� Release()�� ȣ�� �ؾ��Ѵ�.
	// ������ ����ϰ� �Լ� ���ο��� ���� ���Ŷ�� ���ʿ��ϰ� AddRef(), Release()�� ȣ���� �ʿ� ����.
	// ������ �Լ� ȣ���� ������ ���� �޸𸮰� �����Ǿ �ش� ���� ������ ������ �� ���� ����
	template <iunknown T>
	T* GetResource(unsigned int handle);

	unsigned int GetIndexBufferSize(EResourceType indexBufferSizeType);

	// ������� �ʴ� �ڵ����� Ȯ���ϴ� ������
	// builder�� ���� ���õ� �ڵ��� �ִµ� �� ���� �ٸ� �ڵ��� �ٽ� �����ϴ� ��� ���� �ڵ��� ��ȿ�ϸ� Release()�� �ϱ� ����
	bool IsUnusedHandle(unsigned int handle);

	// �ļ��� ���ؼ� mesh object�� �ε��ϴ� ��� resource manager �ܺο��� �ڿ��� �����Ǳ� ������
	// �ܺο��� �� �Լ��� �θ��� �ش� �ڿ��� �����ϱ� ���� IResource�� ������ resource manager ���ο��� �ϱ� ����
public:
	template <iunknown T>
	void RegisterResource(EResourceType eResourceType, T* inputResource);
	void RegisterIndexBufferSize(EResourceType eIndexBufferSizeType, unsigned int indexBufferSize);

private:
	void CreateBuffers();
	void CreateShadersAndInputLayout();

private:
	void CreateGridBuffer();
	void CreateWorldAxisBuffer();
	void CreateBoxBuffer();
	void CreateConstantBufferAll();

	void CreateVertexShaderAndInputLayoutAll();
	void CreatePixelShaderAll();

	void CreateSampler();
	void CreateTexture();

private:
	std::unordered_map<EResourceType, unsigned int> m_resourceTypeToHandleTable;
	std::unordered_map<unsigned int, IResource*> m_handleToResourceTable;
	std::unordered_map<EResourceType, unsigned int> m_IndexBufferSizes;

	unsigned int m_handleNumber;

	// constexpr�� ������ Ÿ�ӿ� ����� Ȯ���ؾ� �ϴµ� mStartHandleNumber�� ��� �����̰� �̴� Ŭ������ �ν��Ͻ�ȭ �� �� ����� ���õǱ� ������ ����
	// ��Ÿ�ӿ� ���ȭ�� ��Ű�� ��� ������ ������ Ÿ�ӿ� ���ȭ�� ��Ű�� constexpr���� �浹 ���°� �翬��
	//constexpr unsigned int mStartHandleNumber = 1000;

	// constexpr�� static�� �ٰ� �Ǹ� Ŭ���� �ν��Ͻ��ʹ� ��� ���� ������ ������ �ڸ��� ��� ������ Ÿ�ӿ� ��� ���� Ȯ���� �� ����
	static constexpr unsigned int mStartHandleNumber = 1000;

	// const�� ������ Ÿ�ӿ� ���ȭ �� ���� ��Ÿ�ӿ� ���ȭ �� ���� �ִµ�, ���⼭ const�� ������ Ÿ�� ���ȭ�� �ƴ� ��Ÿ�� ���ȭ��.
	// ��� ������ ��Ÿ�ӿ� Ŭ������ �ν��Ͻ�ȭ �ɶ� ��μ� ���� �Ҵ�Ǹ鼭 ���ȭ�� �Ǳ� ����
	//const unsigned int mStartHandleNumber = 1000;

	// ������� ���� �ڿ��� ���ؼ��� �ڵ� ���� 0���� �༭ �ڿ��� ���� �����͸� �Ҵ� �޾� ����ϴ� ��찡 ������ ����
public:
	static constexpr unsigned int mUnusedHandleNumber = 0;

	// resource manager������ ���� ����� Device ������
private:
	ID3D11Device* m_pDevice;

private:
	ASEParser* mASEParser;
	ObjectResourceLoadingClass* mObjectResourceLoadingClass;
};

template<iunknown T>
inline T* ResourceManager::GetResource(unsigned int handle)
{
	auto it = m_handleToResourceTable.find(handle);

	assert(it != m_handleToResourceTable.end());

	// ����
	// it->second�� IResource�� �����͸� ����Ѵٰ� �����ؼ�
	// class IReosurce ���漱���� �ߴµ� dynamic_cast<>(��)�� ������ �ִٰ� ����� ��
	// #include IResource.h�� ���� ������ �˷��ִ� ���� ���� ����

	// dynamic_cast<>() �� ���� ���ڷ� ���� �������� ���ΰ� ��� ������� ��Ȯ�� �˾ƾ� �ϴ� �� ����.
	// ��� ���� ģ������ ���θ� �˾ƾ� �갡 ĳ������ �������� �ƴ��� �Ǵ��ϴ� ���� �翬�� �� ����.

	// �ٸ� �θ�(ParentResource)�� ĳ�����ؼ� �ڿ��� ���� ������ ��ȯ
	ParentResource<T>* parentResource = dynamic_cast<ParentResource<T>*>(it->second);
	assert(parentResource != nullptr);

	return parentResource->Get();
}

// wrl/clinet.h �� comptr ����
// Get() -> raw pointer ��ȯ
// GetAddressOf() -> �ڿ��� ���� �������� �ּ� ��ȯ
// operator& -> Release(���� ī��Ʈ ����)�ϸ鼭 comptr�� ���� ����(�������� �ּ�) ��ȯ(ReleaseAndGetAddress�� ������ ����)
// Detach() -> �����ϴ� �������̽��� ���� �����͸� nullptr ���������� ���� ī��Ʈ ���� x
// Attach() -> ���ڷ� ���� �����͸� ���� ������� ���������� ���� ī��Ʈ ���� x
// Reset() -> �����ϴ� ��� ���� �����͸� nullptr�� �����ϰ� ���� ī��Ʈ�� ���� ��Ŵ

template<iunknown T>
inline void ResourceManager::RegisterResource(EResourceType eResourceType, T* inputResource)
{
	m_resourceTypeToHandleTable.insert(std::make_pair(eResourceType, m_handleNumber));
	
	IResource* pResource = new ChildResource<T>(inputResource);
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));

	++m_handleNumber;
}
