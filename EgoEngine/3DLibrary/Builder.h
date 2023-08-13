#pragma once

// 2023 06 16 이정모 home

// 리소스 매니저로부터 자원에 대한 핸들을 가져와 빌더에 세팅한다.
// 빌더는 Object Type을 알고 있기 때문에 핸들을 가지고 실제 오브젝트를 생성하고 이를 factory에 넘긴다.

// IBuilder 인터페이스를 만들지 않은 이유는
// 추상화를 사용한다는 것은 같은 인터페이스를 가지고서 다형적 동작을 하기 위함이다.
// mesh type(non mesh, mesh, skinned mesh)마다 세팅 해야하는 핸들(Texture, Sampler 등)이 모두 다른 상황인데, 
// 인터페이스에서 이를 다 관리하게 되면 특정 오브젝트는 세팅할 필요가 없는 SRV, Sampler 등에 대한 Set() 함수도 가져가야 하기 떄문에 추상화를 하지 않았다.
// 즉, 시그니처가 달라서 추상화를 할 수가 없다고 판단.
// 대신 클래스 템플릿 부분 특수화를 통해 해당 오브젝트의 mesh type에 맞는 builder가 factory 내부에서 자동으로 생성되게 만들었다.

// 장점은 사용자가 factory만 만들면 내부에서 제공해주는 builder를 세팅해서 CreateObject()를 호출하면 끝이다.
// 또한 생성하고자 하는 오브젝트의 mesh type(non mesh, mesh, skinned mesh)을 자동으로 찾아서
// 그에 맞는 builder를 특수화해서 생성하기 때문에 불필요한 함수나 변수(SRV, Sampler 등)를 아예 만들지 않았고 사용자는 있는 함수만 그냥 쓰면 되도록 하였다.

template <typename ObjectType, typename MeshType>
class Builder
{
public:
	Builder<ObjectType, MeshType>();
	~Builder<ObjectType, MeshType>();
};

template<typename ObjectType, typename MeshType>
inline Builder<ObjectType, MeshType>::Builder()
{
}

template<typename ObjectType, typename MeshType>
inline Builder<ObjectType, MeshType>::~Builder()
{
}
