#pragma once
#include <list>
#include <cassert>

class IObject;

// 2023 07 15 ������ home

// �Ϲ����� new/delete�� ���� �Ź� �ü������ �޸� �Ҵ�/������ �䱸���� �ʰ�
// malloc()���� �ѹ��� ū �޸𸮸� �Ҵ� ���� ��
// �̹� �Ҵ� ���� �޸𸮿� new(placement new)/delete�� �Ͽ� �ӵ� ����� �ϱ� ���� object pool

template <typename ObjectType>
class ObjectPool
{
public:
	ObjectPool();
	~ObjectPool();

	void Initialize();
	void Finalize();

public:
	void* Allocate();
	void DeAllocate(void* pObj);

private:
	// malloc()���� �ϳ��� �޸� chunk�� �Ҵ� ���� ���� �ּ�
	void* mAllocatedMemoryStartAddress;

	// �޸� chunk�� object type ũ��� �ɰ��� ������ ���� �ּҸ� ����
	std::list<void*> mFreeList;

	constexpr static int mPoolSize = 10;
};

template<typename ObjectType>
inline ObjectPool<ObjectType>::ObjectPool()
	: mAllocatedMemoryStartAddress(nullptr)
	, mFreeList()
{
}

template<typename ObjectType>
inline ObjectPool<ObjectType>::~ObjectPool()
{
	assert(mFreeList.size() == mPoolSize);

	mFreeList.clear();
}

template<typename ObjectType>
inline void ObjectPool<ObjectType>::Initialize()
{
	mAllocatedMemoryStartAddress = malloc(sizeof(ObjectType) * mPoolSize);

	// malloc()�� ���� �Ҵ� ���� �ּҸ� ��������
	// object type��ŭ �ٸ鼭 ���� �ּҸ� �� ����
	ObjectType* ptr = reinterpret_cast<ObjectType*>(mAllocatedMemoryStartAddress);
	for (int i = 0; i < mPoolSize; ++i, ++ptr)
	{
		mFreeList.push_back(reinterpret_cast<void*>(ptr));
	}
}

template<typename ObjectType>
inline void ObjectPool<ObjectType>::Finalize()
{
	free(mAllocatedMemoryStartAddress);
}

template<typename ObjectType>
inline void* ObjectPool<ObjectType>::Allocate()
{
	// ��� ������ �޸𸮰� ���ٸ� malloc()�� ���� �� �Ҵ� �޾ƾ� ��
	// �ϴ��� assert()
	assert(mFreeList.size() != 0);

	void* ptr = mFreeList.front();
	mFreeList.pop_front();

	return ptr;
}

template<typename ObjectType>
inline void ObjectPool<ObjectType>::DeAllocate(void* pObj)
{
	mFreeList.push_back(pObj);
}
