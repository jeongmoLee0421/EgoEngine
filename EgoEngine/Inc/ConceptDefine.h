#pragma once
#include <Unknwnbase.h>

// 2023 07 15 이정모 home

// 클래스 템플릿이나 함수 템플릿을 사용할 떄 제약 조건을 넣기 위해서
// 제약 조건들을 묶어 놓은 concept을 정의

// requires 표현식
// 매개변수도 있고 내부에서 객체를 통한 함수 호출문도 있는 것으로 보아 함수처럼 생겼지만
// 런타임에 실행되지 않음. 컴파일 타임에 검사하는 것임
template <typename T>
concept iunknown = requires(T t)
{
	// 실제로 함수가 호출되는 것도 아니고, 매개변수에 객체가 생성되서 전달 되는 것도 아님.
	// 이러한 형식의 함수가 있는지 컴파일 타임에 검사할 뿐
	t.QueryInterface(IID{}, nullptr);
	t.AddRef();
	t.Release();
};