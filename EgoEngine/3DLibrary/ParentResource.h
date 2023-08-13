#pragma once

#include "../Inc/DX11Define.h"
//#include <vcruntime_typeinfo.h>

// 2023 06 22 ������ home

// ��� �ڿ��� ���� ������ IResource�� �ϸ鼭 dx�ڷ����� ����
// ParentResource�� ���� �ڿ��� ���� �����Ͱ� �����ϴ� class��
// �ڿ��� ��û�� �� ParentResource�� dynamic_cast<>()�Ͽ� �ش� �ڿ��� �����͸� �Ѱ���

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
	//  IUnkown�� ��ӹ��� dx�ڿ��鸸 T�� �����ϱ� ����
	m_resource->Release();
	// <vcruntime_typeinfo.h>�� include�ϸ� ��Ÿ�ӿ� T�� Ÿ���� Ȯ���Ͽ� Ÿ�� �񱳵� ����
	/*if (typeid(T) == typeid(ID3D11Device))
	{

	}*/
}

template<typename T>
inline T* ParentResource<T>::Get()
{
	return m_resource;
}
