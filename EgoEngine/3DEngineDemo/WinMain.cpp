#include "GameProcess.h"
#include <Windows.h>

// 2023/06/09 ������ home

// ���� ������

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	GameProcess* pGameProcess = new GameProcess();

	if (FAILED(pGameProcess->Initialize(hInstance, nCmdShow))) return -1;

	pGameProcess->Loop();

	if(FAILED(pGameProcess->Finalize())) return -1;

	delete pGameProcess;

	return 0;
}