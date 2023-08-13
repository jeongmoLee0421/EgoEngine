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

// 2023 06 13 이정모 home

// 공통적으로 사용하는
// vertex buffer, index buffer, constant buffer,
// vertex shader, input layout, pixel shader,
// texture 등을 로드하고 관리하는 매니저 클래스

// 객체 생성을 위한 빌더를 세팅할 때 핸들을 넘겨줌
// 객체들은 필요할 때만 핸들을 기반으로 자원에 대한 포인터를 얻어와서 사용

// 자원 관리 방법
// 1. 빌더 제작 시 handle을 Set 하는 과정에서 AddRef()
// 빌더 소멸 시 소멸자에서 Release()

// 2. 빌더를 기반으로 팩토리가 오브젝트를 생성하면서 생성자로 핸들이 넘어가고
// Initialize()에서 해당 핸들이 사용하는 핸들이라면 AddRef()
// 객체 소멸 전 Finalize()에서 사용하는 핸들에 대해서만 Release()

// 3. 모든 자원에 대한 참조가 해제되고
// 마지막으로 리소스 매니저가 핸들-리소스 맵을 순회하면서 모든 자원에 대해 Release() 호출

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
	// 자원에 대한 핸들을 요청하는 것은 객체가 살아있는 동안에는 언제든지 자원을 찾을 수 있기 때문에 AddRef()
	unsigned int GetHandleResource(EResourceType eResourceType);

	// 자원에 대한 포인터를 요청할 때
	// 만약 멤버변수로 계속 들고 있을거라면 언제든지 자원을 참조할 수 있기 때문에 AddRef()를 하고 소멸할 때 Release()를 호출 해야한다.
	// 하지만 깔끔하게 함수 내부에서 쓰고 말거라면 불필요하게 AddRef(), Release()를 호출할 필요 없다.
	// 어차피 함수 호출이 끝나면 스택 메모리가 해제되어서 해당 지역 변수는 참조할 수 없기 때문
	template <iunknown T>
	T* GetResource(unsigned int handle);

	unsigned int GetIndexBufferSize(EResourceType indexBufferSizeType);

	// 사용하지 않는 핸들인지 확인하는 이유는
	// builder에 먼저 세팅된 핸들이 있는데 이 위에 다른 핸들을 다시 세팅하는 경우 이전 핸들이 유효하면 Release()를 하기 위함
	bool IsUnusedHandle(unsigned int handle);

	// 파서를 통해서 mesh object를 로드하는 경우 resource manager 외부에서 자원이 생성되기 때문에
	// 외부에서 이 함수를 부르고 해당 자원을 관리하기 위한 IResource를 생성은 resource manager 내부에서 하기 위함
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

	// constexpr은 컴파일 타임에 상수를 확정해야 하는데 mStartHandleNumber는 멤버 변수이고 이는 클래스가 인스턴스화 될 때 상수가 세팅되기 때문에 에러
	// 런타임에 상수화를 시키는 멤버 변수는 컴파일 타임에 상수화를 시키는 constexpr과는 충돌 나는게 당연함
	//constexpr unsigned int mStartHandleNumber = 1000;

	// constexpr에 static이 붙게 되면 클래스 인스턴스와는 상관 없이 데이터 영역에 자리를 잡고 컴파일 타임에 상수 값을 확정할 수 있음
	static constexpr unsigned int mStartHandleNumber = 1000;

	// const는 컴파일 타임에 상수화 될 수도 런타임에 상수화 될 수도 있는데, 여기서 const는 컴파일 타임 상수화가 아닌 런타임 상수화임.
	// 멤버 변수는 런타임에 클래스가 인스턴스화 될때 비로소 값이 할당되면서 상수화가 되기 때문
	//const unsigned int mStartHandleNumber = 1000;

	// 사용하지 않을 자원에 대해서는 핸들 값을 0으로 줘서 자원에 대한 포인터를 할당 받아 사용하는 경우가 없도록 하자
public:
	static constexpr unsigned int mUnusedHandleNumber = 0;

	// resource manager에서만 직접 사용할 Device 포인터
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

	// 주의
	// it->second로 IResource의 포인터만 사용한다고 생각해서
	// class IReosurce 전방선언을 했는데 dynamic_cast<>(식)에 오류가 있다고 출력이 뜸
	// #include IResource.h로 명세를 완전히 알려주니 문제 없이 동작

	// dynamic_cast<>() 할 때는 인자로 들어온 포인터의 내부가 어떻게 생겼는지 정확히 알아야 하는 것 같다.
	// 어떻게 생긴 친구인지 내부를 알아야 얘가 캐스팅이 가능한지 아닌지 판단하는 것이 당연한 것 같다.

	// 다른 부모(ParentResource)로 캐스팅해서 자원에 대한 포인터 반환
	ParentResource<T>* parentResource = dynamic_cast<ParentResource<T>*>(it->second);
	assert(parentResource != nullptr);

	return parentResource->Get();
}

// wrl/clinet.h 의 comptr 정리
// Get() -> raw pointer 반환
// GetAddressOf() -> 자원에 대한 포인터의 주소 반환
// operator& -> Release(참조 카운트 감소)하면서 comptr의 약한 참조(포인터의 주소) 반환(ReleaseAndGetAddress와 동일한 동작)
// Detach() -> 관리하는 인터페이스에 대한 포인터를 nullptr 세팅하지만 참조 카운트 감소 x
// Attach() -> 인자로 들어온 포인터를 관리 대상으로 세팅하지만 참조 카운트 증가 x
// Reset() -> 관리하는 대상에 대한 포인터를 nullptr로 세팅하고 참조 카운트를 감소 시킴

template<iunknown T>
inline void ResourceManager::RegisterResource(EResourceType eResourceType, T* inputResource)
{
	m_resourceTypeToHandleTable.insert(std::make_pair(eResourceType, m_handleNumber));
	
	IResource* pResource = new ChildResource<T>(inputResource);
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));

	++m_handleNumber;
}
