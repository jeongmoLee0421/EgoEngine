#pragma once

#include <Windows.h>
#include <vector>

// 2023/06/09 이정모 home

// 게임 진행 클래스
// 윈도우 생성 / 게임 루프

class IRenderer;
class Timer;
class ICamera;
class ResourceManager;
class IObject;

// class IFactory;

// class template 전방 선언
template <typename ObjectType>
class Factory;

class Grid;
class WorldAxis;
class Box;
class MeshObject;

class GameProcess
{
public:
	GameProcess();
	~GameProcess();

	HRESULT Initialize(HINSTANCE hInstance, int nCmdShow);
	HRESULT Finalize();

	void Loop();

	static LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Render();

private:
	IRenderer* m_pDXRenderer;
	Timer* m_pTimer;

	ICamera* m_pDXCamera;
	static ICamera* m_pDXCameraInstance; // static 멤버 함수 winproc에서 사용

	ResourceManager* m_pResourceManager;

	// 생성된 모든 object를 관리하는 리스트로
	// update(), render()의 대상들
	std::vector<IObject*> mAllObjects;

	// 생성한 object를 올바른 factory에 반납하기 위한 리스트
	std::vector<IObject*> mGridObjects;
	std::vector<IObject*> mWorldAxisObjects;
	std::vector<IObject*> mBoxObjects;
	std::vector<IObject*> mMeshObjects;

	// factory 내부에서 object type에 맞는 전용 builder<>를 생성하기 위해 IFactory를 사용하지 않았음
	Factory<Grid>* mGridFactory;
	Factory<WorldAxis>* mWorldAxisFactory;
	Factory<Box>* mBoxFactory;
	Factory<MeshObject>* mMeshObjectFactory;

private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	MSG m_msg;

	LONG m_clientWidth;
	LONG m_clientHeight;
};

