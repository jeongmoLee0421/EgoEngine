#pragma once

// 2023 06 16 ������ home

// ���ҽ� �Ŵ����κ��� �ڿ��� ���� �ڵ��� ������ ������ �����Ѵ�.
// ������ Object Type�� �˰� �ֱ� ������ �ڵ��� ������ ���� ������Ʈ�� �����ϰ� �̸� factory�� �ѱ��.

// IBuilder �������̽��� ������ ���� ������
// �߻�ȭ�� ����Ѵٴ� ���� ���� �������̽��� ������ ������ ������ �ϱ� �����̴�.
// mesh type(non mesh, mesh, skinned mesh)���� ���� �ؾ��ϴ� �ڵ�(Texture, Sampler ��)�� ��� �ٸ� ��Ȳ�ε�, 
// �������̽����� �̸� �� �����ϰ� �Ǹ� Ư�� ������Ʈ�� ������ �ʿ䰡 ���� SRV, Sampler � ���� Set() �Լ��� �������� �ϱ� ������ �߻�ȭ�� ���� �ʾҴ�.
// ��, �ñ״�ó�� �޶� �߻�ȭ�� �� ���� ���ٰ� �Ǵ�.
// ��� Ŭ���� ���ø� �κ� Ư��ȭ�� ���� �ش� ������Ʈ�� mesh type�� �´� builder�� factory ���ο��� �ڵ����� �����ǰ� �������.

// ������ ����ڰ� factory�� ����� ���ο��� �������ִ� builder�� �����ؼ� CreateObject()�� ȣ���ϸ� ���̴�.
// ���� �����ϰ��� �ϴ� ������Ʈ�� mesh type(non mesh, mesh, skinned mesh)�� �ڵ����� ã�Ƽ�
// �׿� �´� builder�� Ư��ȭ�ؼ� �����ϱ� ������ ���ʿ��� �Լ��� ����(SRV, Sampler ��)�� �ƿ� ������ �ʾҰ� ����ڴ� �ִ� �Լ��� �׳� ���� �ǵ��� �Ͽ���.

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
