#pragma once

#include "../Inc/NonMeshBuilder.h"
#include "../Inc/MeshBuilder.h"
#include "../Inc/ObjectPool.h"
#include "../Inc/IObject.h"

// 2023 06 18 이정모 home

// builder가 오브젝트를 생성해주면 이를 외부로 넘겨주는 class

// 원래는 IFactory로 추상화해서 어떤 object를 생성하는 factory라도 다 받을 수 있도록 관리하고 있었으나
// builder<object type, mesh type>을 반환하는 GetBuilder() 함수를 interface에서 사용하려면 특정 object type을 전해줘야 하는데
// 구체적인 타입을 IFactory에 전해주는 순간 interface가 특정 object type에 종속되어버림.
// 즉 IFactory* 에서 IFactory<ObjectType>*가 되어버림.
// 어차피 IFactory가 type에 종속될바에 factory에 넣어준 object type을 기반으로 그에 맞는 builder가 자동으로 생성되는 것이 더 효율적인 방안이라 생각해서 이를 선택

// builder<>를 반환하려면 반드시 구체적인 타입이 필요하고 이를 interface에서 사용하면 특정 object type에 종속적이라서 IBuilder 사용도 고려해 보았으나
// IBuilder도 결국 non mesh 오브젝트에는 불필요한 SetTexture(), SetSampler()를 가지고 있어야 하게 된다.
// 즉, 시그니처가 다르기 때문에 인터페이스로 묶을 수 없음.

// 더 좋은 방법이 있을까?

class ResourceManager;

template <typename ObjectType>
class Factory
{
public:
	Factory(ResourceManager* pResourceManager);
	~Factory();

	void Initialize();
	void Finalize();

public:
	// object type이 추가된다고 해서 새로운 생산 함수를 만들거나 내부를 수정할 필요가 없음.
	// 왜? Factory에서 템플릿 인자로 받은 object type을 통해 그에 맞는 builder가 생성되고
	// builder는 이미 ObjectType을 알기 때문에 object를 생성하는 방법을 내부적으로 알고 있고
	// 그 builer만 채워서 CreateObject()를 호출하면 추상화된 IObject를 반환하기 때문에
	// object type에 비종속적으로 동작함
	IObject* CreateObject();

	void ReturnObject(IObject* pObj);

	// object type과 mesh type으로 자동 생성된 builder를 참조하기 위한 Get()
	Builder<ObjectType, typename ObjectType::MeshType>* GetBuilder();

private:
	ResourceManager* m_pResourceManager;

	// ObjectType 내부에 using문을 사용하여 자신이 상속받은 mesh type을 재정의 해둔 것을 가져옴
	// 이로써 factory가 object type을 받으면 이를 기준으로 mesh type을 가져와서 전용 builder를 자동으로 생성
	// ObjectType::MeshType과 같이 타입에 종속(::)적인 이름은 컴파일러가 변수로 해석할 수 있기 때문에 typename 키워드로 명시하여 타입임을 알려줌
	Builder<ObjectType, typename ObjectType::MeshType>* mBuilder;

	// object pool은 malloc()을 통해 한번에 큰 메모리 chunk를 할당 받아 두고서 직접 메모리 관리를 한다.
	// 매번 운영체제에게 메모리를 요청하는 new()를 호출하는 것이 아니라
	// 이미 할당해둔 주소를 반환하는 placement new()를 호출함으로써 속도에서 이점을 얻자
	ObjectPool<ObjectType>* mObjectPool;
};

template <typename ObjectType>
Factory<ObjectType>::Factory(ResourceManager* pResourceManager)
	: m_pResourceManager(pResourceManager)
	, mBuilder(nullptr)
	, mObjectPool(nullptr)
{

}

template <typename ObjectType>
Factory<ObjectType>::~Factory()
{

}

template <typename ObjectType>
void Factory<ObjectType>::Initialize()
{
	mBuilder = new Builder<ObjectType, typename ObjectType::MeshType>(m_pResourceManager);

	mObjectPool = new ObjectPool<ObjectType>();
	mObjectPool->Initialize();
}

template <typename ObjectType>
void Factory<ObjectType>::Finalize()
{
	delete mBuilder;
	mBuilder = nullptr;

	mObjectPool->Finalize();
	delete mObjectPool;
	mObjectPool = nullptr;
}

template <typename ObjectType>
IObject* Factory<ObjectType>::CreateObject()
{
	// 과거에는 매개변수 없이 build()를 호출했고 운영체제에게 주소를 요청했지만
	// 이제는 매개변수로 object pool을 넘겨서 이미 할당 받은 주소를 사용하도록 변경
	return mBuilder->Build(mObjectPool);
}

template<typename ObjectType>
inline void Factory<ObjectType>::ReturnObject(IObject* pObj)
{
	pObj->Finalize();

	// 생성자에서 정상적으로 객체가 초기화된 이후 정상적인 반납 절차를 거치게 되면 ReturnObject()에서 DeAllocate()를 호출할 것이고
	// 생성자에서 예외를 throw하면 사용자에게 Object의 유효한 address가 return되지 않을 것이고
	// 컴파일러가 내부에서 new()와 짝이 맞는 delete()를 찾아서 호출할 것이고 그 짝이 맞는 delete()에서 DeAllocate()를 호출한다.
	// class 내부에서 오버로딩한 placement delete()는 명시적으로 호출이 불가능하다. 즉, 예외가 발생했을 때 오직 컴파일러만이 호출하는 것이다.
	// 만약 placement delete()만 오버로딩한 상태에서 usual delete()를 호출하면 컴파일 에러가 발생한다.
	// 왜냐하면 전역 delete()가 class 내부에서 오버로딩한 placement delete()에 의해 가려졌기 때문이다.
	// https://en.cppreference.com/w/cpp/memory/new/operator_delete의 Class-specific overloads부분
	// 그래서 usual delete()도 class 내부에 오버로딩을 직접 해줘야 호출이 가능하다.
	mObjectPool->DeAllocate(pObj);
	
	// usual delete 연산자
	delete pObj;
}

template<typename ObjectType>
inline Builder<ObjectType, typename ObjectType::MeshType>* Factory<ObjectType>::GetBuilder()
{
	// 물론 factory 하나만 있으면 오브젝트를 생성할 수 있지만
	// factory가 복사되는 경우 builder를 포인터 멤버 변수로 유지하거나 일반 멤버 변수로 유지하거나 상관없이
	// 결국 핸들이 복사되어야 하기 때문에 factory의 복사 생성자나 복사 대입연산자에서는 AddRef()를 해야함.
	// factory의 copy 자체를 막는 것도 하나의 방법.
	
	// 나는 실제 데이터는 heap에 두고 오브젝트는 가볍게 포인터 변수만 들고 있는걸 좋아함.
	// 그리고 포인터는 내가 원하는 타이밍에 할당/해제가 가능한 것도 좋음.
	// 다만 포인터를 들고 있을거면 복사연산 때는 heap에 있는 동일한 데이터를 가리키지 않도록
	// 포인터가 가리키는 실제 데이터 전체를 복사하여 새로운 heap영역에 할당하고 그 곳을 가리키도록 해야한다.

	return mBuilder;
}
