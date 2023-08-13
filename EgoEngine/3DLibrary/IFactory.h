#pragma once

#include "../Inc/Builder.h"

// 2023 06 19 이정모 home

// 일반화한 factory가 어떤 오브젝트 클래스에도 종속적이지 않게 추상화해서 받기 위한 인터페이스

// Factory를 일반화해서 어떤 오브젝트라도 생성할 수 있게 해두었는데
// IFactory로 추상화해서 받아두지 않으면
// Factory<ObjectClass>* var; 형태로 내가 생성할 클래스의 팩토리 자료형이 이미 정해져 있게됨
// 인터페이스를 통해 부모 포인터로 받아서 해결

class IObject;

// 원래 IFactory를 사용해서 팩토리를 추상화해서 관리했지만
// Builder를 반환해주는 GetBuilder()에서 Builder<>의 구체적인 타입이 정해져야 하고
// 이 함수를 Interface에서 사용하려면 IFactory가 특정 object type에 종속적이게 되어버림(IFactory* -> IFactory<object type>*)
// 나는 IFactory를 사용해서 factory들을 추상화하는 것보다 object type과 mesh type에 적합한 builder를 자동으로 생성해주는게 더 중요하다고 판단함.
// 이전에는 builder class가 하나만 존재해서 특정 오브젝트(grid...)에는 불필요한 핸들(SRV, Sampler)까지 넘겨줘야 했으나 이젠 그럴 필요가 없음
// 클래스 템플릿 특수화를 통해 아예 함수를 만들지 않았음
class IFactory
{
public:
	IFactory() {}
	virtual ~IFactory() = 0 {}

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

public:
	virtual IObject* CreateObject() = 0;
};