#pragma once
#include <list>
#include <cassert>

class IObject;

// 2023 07 15 이정모 home

// 일반적인 new/delete를 통해 매번 운영체제에게 메모리 할당/해제를 요구하지 않고
// malloc()으로 한번에 큰 메모리를 할당 받은 뒤
// 이미 할당 받은 메모리에 new(placement new)/delete를 하여 속도 향상을 하기 위한 object pool

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
	// malloc()으로 하나의 메모리 chunk를 할당 받은 시작 주소
	void* mAllocatedMemoryStartAddress;

	// 메모리 chunk를 object type 크기로 쪼개서 각각의 시작 주소를 보관
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

	// malloc()을 통해 할당 받은 주소를 시작으로
	// object type만큼 뛰면서 시작 주소를 다 저장
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
	// 사용 가능한 메모리가 없다면 malloc()을 통해 더 할당 받아야 함
	// 일단은 assert()
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
