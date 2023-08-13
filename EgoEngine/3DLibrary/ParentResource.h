#pragma once

#include "../Inc/DX11Define.h"
//#include <vcruntime_typeinfo.h>

// 2023 06 22 이정모 home

// 모든 자원에 대한 관리는 IResource로 하면서 dx자료형은 숨김
// ParentResource가 실제 자원에 대한 포인터가 존재하는 class로
// 자원을 요청할 때 ParentResource로 dynamic_cast<>()하여 해당 자원의 포인터를 넘겨줌

template <typename T>
class ParentResource
{
public:
	ParentResource(T* ptr);
	virtual ~ParentResource();

	T* Get();

private:
	T* m_resource;
};

template<typename T>
inline ParentResource<T>::ParentResource(T* ptr)
	:m_resource(ptr)
{
	m_resource->AddRef();
}

template<typename T>
inline ParentResource<T>::~ParentResource()
{
	//  IUnkown을 상속받은 dx자원들만 T로 들어오니까 가능
	m_resource->Release();
	// <vcruntime_typeinfo.h>를 include하면 런타임에 T의 타입을 확인하여 타입 비교도 가능
	/*if (typeid(T) == typeid(ID3D11Device))
	{

	}*/
}

template<typename T>
inline T* ParentResource<T>::Get()
{
	return m_resource;
}
