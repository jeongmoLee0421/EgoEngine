#pragma once

#include "../Inc/IResource.h"
#include "../Inc/ParentResource.h"
#include "../Inc/ConceptDefine.h"

// 2023 06 22 ������ home

// ��� �ִ� Ŭ������ IResource�� ParentResource�� ���� ���

// IResource�θ� �����ϸ� Get()�Լ� ������ Interface���� ��ȯ ������ �ڿ��� ���� �ڷ���(id3d11device ��)�� ����Ǳ� ������
// dynamic_cast<>�� ���� ParentResource�� cross type casting(�ٸ� �θ�� ��ĳ����)�� �Ͽ� Get()�Լ��� �����.
// ����� IResource�� ParentResource�� ��Ӱ��谡 �ƴ϶� static_cast<>�� �Ұ�

// ������ IResource�� �ϰ����ְ� �ϳ��� �ڷ������� �ϸ鼭 �ڿ��� ���� �����͸� ��ȯ�ؾ� �Ҷ��� ParentResource�� ���

// dynamic_cast�� ���� ���� ���ϰ� ������
// ResourceManager �ϳ��� ��� �ڿ��� �ϳ��� �������̽��� �ϰ��� �ְ� �����ϴ°� ���� �� ���ٰ� �Ǵ���
// Ư���� �ϳ��� �ڿ��� �����ϴ� ResourceManager<T>�� ���� ���� �ʿ䰡 ���°� �� ����

// Child Resource�� DX �ڿ��� ���� �����͸� �޾Ƽ� �����ϴ� class��
// ������ DX �ڿ��� �ƴ� �ٸ� �ڷ����� ���� �����Ͱ� ���� �� �ְ�
// ������ Ÿ�ӿ� ������ �߻����� �� ������ �� ģ���ϰ� ���� �޽����� ��� �ִ� ���������� ������ concept�� ���
// ���� �ؼ��ϱ� ������ type_traits�� ������ �����Ǿ� �ִ� �ͺ��� �б� ����
// DX �ڿ��� Iunknown�� ��� �ް� �ֱ� ������ Iunknown���� �Ļ��� �Լ����� �ִ��� �˻�����
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