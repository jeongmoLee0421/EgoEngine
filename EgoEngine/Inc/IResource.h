#pragma once

// 2023 06 22 이정모 home

// 모든 자원들을 일관성있게 관리하기 위한 인터페이스

class IResource
{
public:
	IResource() {}
	virtual ~IResource() = 0 {}
};