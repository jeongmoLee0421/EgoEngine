#pragma once

#include "../Inc/Builder.h"

// 2023 06 19 ������ home

// �Ϲ�ȭ�� factory�� � ������Ʈ Ŭ�������� ���������� �ʰ� �߻�ȭ�ؼ� �ޱ� ���� �������̽�

// Factory�� �Ϲ�ȭ�ؼ� � ������Ʈ�� ������ �� �ְ� �صξ��µ�
// IFactory�� �߻�ȭ�ؼ� �޾Ƶ��� ������
// Factory<ObjectClass>* var; ���·� ���� ������ Ŭ������ ���丮 �ڷ����� �̹� ������ �ְԵ�
// �������̽��� ���� �θ� �����ͷ� �޾Ƽ� �ذ�

class IObject;

// ���� IFactory�� ����ؼ� ���丮�� �߻�ȭ�ؼ� ����������
// Builder�� ��ȯ���ִ� GetBuilder()���� Builder<>�� ��ü���� Ÿ���� �������� �ϰ�
// �� �Լ��� Interface���� ����Ϸ��� IFactory�� Ư�� object type�� �������̰� �Ǿ����(IFactory* -> IFactory<object type>*)
// ���� IFactory�� ����ؼ� factory���� �߻�ȭ�ϴ� �ͺ��� object type�� mesh type�� ������ builder�� �ڵ����� �������ִ°� �� �߿��ϴٰ� �Ǵ���.
// �������� builder class�� �ϳ��� �����ؼ� Ư�� ������Ʈ(grid...)���� ���ʿ��� �ڵ�(SRV, Sampler)���� �Ѱ���� ������ ���� �׷� �ʿ䰡 ����
// Ŭ���� ���ø� Ư��ȭ�� ���� �ƿ� �Լ��� ������ �ʾ���
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