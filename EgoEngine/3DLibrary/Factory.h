#pragma once

#include "../Inc/NonMeshBuilder.h"
#include "../Inc/MeshBuilder.h"
#include "../Inc/ObjectPool.h"
#include "../Inc/IObject.h"

// 2023 06 18 ������ home

// builder�� ������Ʈ�� �������ָ� �̸� �ܺη� �Ѱ��ִ� class

// ������ IFactory�� �߻�ȭ�ؼ� � object�� �����ϴ� factory�� �� ���� �� �ֵ��� �����ϰ� �־�����
// builder<object type, mesh type>�� ��ȯ�ϴ� GetBuilder() �Լ��� interface���� ����Ϸ��� Ư�� object type�� ������� �ϴµ�
// ��ü���� Ÿ���� IFactory�� �����ִ� ���� interface�� Ư�� object type�� ���ӵǾ����.
// �� IFactory* ���� IFactory<ObjectType>*�� �Ǿ����.
// ������ IFactory�� type�� ���ӵɹٿ� factory�� �־��� object type�� ������� �׿� �´� builder�� �ڵ����� �����Ǵ� ���� �� ȿ������ ����̶� �����ؼ� �̸� ����

// builder<>�� ��ȯ�Ϸ��� �ݵ�� ��ü���� Ÿ���� �ʿ��ϰ� �̸� interface���� ����ϸ� Ư�� object type�� �������̶� IBuilder ��뵵 ����� ��������
// IBuilder�� �ᱹ non mesh ������Ʈ���� ���ʿ��� SetTexture(), SetSampler()�� ������ �־�� �ϰ� �ȴ�.
// ��, �ñ״�ó�� �ٸ��� ������ �������̽��� ���� �� ����.

// �� ���� ����� ������?

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
	// object type�� �߰��ȴٰ� �ؼ� ���ο� ���� �Լ��� ����ų� ���θ� ������ �ʿ䰡 ����.
	// ��? Factory���� ���ø� ���ڷ� ���� object type�� ���� �׿� �´� builder�� �����ǰ�
	// builder�� �̹� ObjectType�� �˱� ������ object�� �����ϴ� ����� ���������� �˰� �ְ�
	// �� builer�� ä���� CreateObject()�� ȣ���ϸ� �߻�ȭ�� IObject�� ��ȯ�ϱ� ������
	// object type�� ������������ ������
	IObject* CreateObject();

	void ReturnObject(IObject* pObj);

	// object type�� mesh type���� �ڵ� ������ builder�� �����ϱ� ���� Get()
	Builder<ObjectType, typename ObjectType::MeshType>* GetBuilder();

private:
	ResourceManager* m_pResourceManager;

	// ObjectType ���ο� using���� ����Ͽ� �ڽ��� ��ӹ��� mesh type�� ������ �ص� ���� ������
	// �̷ν� factory�� object type�� ������ �̸� �������� mesh type�� �����ͼ� ���� builder�� �ڵ����� ����
	// ObjectType::MeshType�� ���� Ÿ�Կ� ����(::)���� �̸��� �����Ϸ��� ������ �ؼ��� �� �ֱ� ������ typename Ű����� ����Ͽ� Ÿ������ �˷���
	Builder<ObjectType, typename ObjectType::MeshType>* mBuilder;

	// object pool�� malloc()�� ���� �ѹ��� ū �޸� chunk�� �Ҵ� �޾� �ΰ� ���� �޸� ������ �Ѵ�.
	// �Ź� �ü������ �޸𸮸� ��û�ϴ� new()�� ȣ���ϴ� ���� �ƴ϶�
	// �̹� �Ҵ��ص� �ּҸ� ��ȯ�ϴ� placement new()�� ȣ�������ν� �ӵ����� ������ ����
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
	// ���ſ��� �Ű����� ���� build()�� ȣ���߰� �ü������ �ּҸ� ��û������
	// ������ �Ű������� object pool�� �Ѱܼ� �̹� �Ҵ� ���� �ּҸ� ����ϵ��� ����
	return mBuilder->Build(mObjectPool);
}

template<typename ObjectType>
inline void Factory<ObjectType>::ReturnObject(IObject* pObj)
{
	pObj->Finalize();

	// �����ڿ��� ���������� ��ü�� �ʱ�ȭ�� ���� �������� �ݳ� ������ ��ġ�� �Ǹ� ReturnObject()���� DeAllocate()�� ȣ���� ���̰�
	// �����ڿ��� ���ܸ� throw�ϸ� ����ڿ��� Object�� ��ȿ�� address�� return���� ���� ���̰�
	// �����Ϸ��� ���ο��� new()�� ¦�� �´� delete()�� ã�Ƽ� ȣ���� ���̰� �� ¦�� �´� delete()���� DeAllocate()�� ȣ���Ѵ�.
	// class ���ο��� �����ε��� placement delete()�� ��������� ȣ���� �Ұ����ϴ�. ��, ���ܰ� �߻����� �� ���� �����Ϸ����� ȣ���ϴ� ���̴�.
	// ���� placement delete()�� �����ε��� ���¿��� usual delete()�� ȣ���ϸ� ������ ������ �߻��Ѵ�.
	// �ֳ��ϸ� ���� delete()�� class ���ο��� �����ε��� placement delete()�� ���� �������� �����̴�.
	// https://en.cppreference.com/w/cpp/memory/new/operator_delete�� Class-specific overloads�κ�
	// �׷��� usual delete()�� class ���ο� �����ε��� ���� ����� ȣ���� �����ϴ�.
	mObjectPool->DeAllocate(pObj);
	
	// usual delete ������
	delete pObj;
}

template<typename ObjectType>
inline Builder<ObjectType, typename ObjectType::MeshType>* Factory<ObjectType>::GetBuilder()
{
	// ���� factory �ϳ��� ������ ������Ʈ�� ������ �� ������
	// factory�� ����Ǵ� ��� builder�� ������ ��� ������ �����ϰų� �Ϲ� ��� ������ �����ϰų� �������
	// �ᱹ �ڵ��� ����Ǿ�� �ϱ� ������ factory�� ���� �����ڳ� ���� ���Կ����ڿ����� AddRef()�� �ؾ���.
	// factory�� copy ��ü�� ���� �͵� �ϳ��� ���.
	
	// ���� ���� �����ʹ� heap�� �ΰ� ������Ʈ�� ������ ������ ������ ��� �ִ°� ������.
	// �׸��� �����ʹ� ���� ���ϴ� Ÿ�ֿ̹� �Ҵ�/������ ������ �͵� ����.
	// �ٸ� �����͸� ��� �����Ÿ� ���翬�� ���� heap�� �ִ� ������ �����͸� ����Ű�� �ʵ���
	// �����Ͱ� ����Ű�� ���� ������ ��ü�� �����Ͽ� ���ο� heap������ �Ҵ��ϰ� �� ���� ����Ű���� �ؾ��Ѵ�.

	return mBuilder;
}
