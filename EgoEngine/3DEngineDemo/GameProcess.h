#pragma once

#include <Windows.h>
#include <vector>

// 2023/06/09 ������ home

// ���� ���� Ŭ����
// ������ ���� / ���� ����

class IRenderer;
class Timer;
class ICamera;
class ResourceManager;
class IObject;

// class IFactory;

// class template ���� ����
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
	static ICamera* m_pDXCameraInstance; // static ��� �Լ� winproc���� ���

	ResourceManager* m_pResourceManager;

	// ������ ��� object�� �����ϴ� ����Ʈ��
	// update(), render()�� ����
	std::vector<IObject*> mAllObjects;

	// ������ object�� �ùٸ� factory�� �ݳ��ϱ� ���� ����Ʈ
	std::vector<IObject*> mGridObjects;
	std::vector<IObject*> mWorldAxisObjects;
	std::vector<IObject*> mBoxObjects;
	std::vector<IObject*> mMeshObjects;

	// factory ���ο��� object type�� �´� ���� builder<>�� �����ϱ� ���� IFactory�� ������� �ʾ���
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

