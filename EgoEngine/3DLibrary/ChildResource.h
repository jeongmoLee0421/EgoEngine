#pragma once

#include "../Inc/IResource.h"
#include "../Inc/ParentResource.h"
#include "../Inc/ConceptDefine.h"

// 2023 06 22 이정모 home

// 비어 있는 클래스로 IResource와 ParentResource를 다중 상속

// IResource로만 관리하면 Get()함수 구현시 Interface에서 반환 값으로 자원에 대한 자료형(id3d11device 등)이 노출되기 때문에
// dynamic_cast<>를 통해 ParentResource로 cross type casting(다른 부모로 업캐스팅)을 하여 Get()함수를 사용함.
// 참고로 IResource와 ParentResource는 상속관계가 아니라 static_cast<>가 불가

// 관리는 IResource로 일관성있게 하나의 자료형으로 하면서 자원에 대한 포인터를 반환해야 할때는 ParentResource를 사용

// dynamic_cast로 인한 성능 저하가 있지만
// ResourceManager 하나가 모든 자원을 하나의 인터페이스로 일관성 있게 관리하는게 나는 더 좋다고 판단함
// 특정한 하나의 자원만 관리하는 ResourceManager<T>를 각각 만들 필요가 없는게 난 좋음

// Child Resource는 DX 자원에 대한 포인터를 받아서 관리하는 class임
// 하지만 DX 자원이 아닌 다른 자료형에 대한 포인터가 들어올 수 있고
// 컴파일 타임에 에러가 발생했을 때 조금은 더 친절하게 에러 메시지를 띄워 주는 제약조건의 묶음인 concept를 사용
// 또한 해석하기 복잡한 type_traits가 여러개 나열되어 있는 것보다 읽기 편함
// DX 자원은 Iunknown을 상속 받고 있기 때문에 Iunknown에서 파생된 함수들이 있는지 검사했음
template <iunknown T>
class ChildResource : public IResource, public ParentResource<T>
{
public:
	ChildResource(T* ptr);
	~ChildResource();
};

template<iunknown T>
inline ChildResource<T>::ChildResource(T* ptr)
	: ParentResource<T>(ptr)
{
}

template<iunknown T>
inline ChildResource<T>::~ChildResource()
{
}