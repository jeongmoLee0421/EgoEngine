#pragma once

#include "../Inc/DirectXTex.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "../Lib/DirectXTex")

// 2023/06/09 ������ home

// dx11 ���� define

// �����ϸ� �����޽����� string table �����ؼ� �۷� ������
#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)	\
{	\
	HRESULT hr = (x);	\
	if(FAILED(hr))	\
	{	\
		LPWSTR output;	\
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |	\
			FORMAT_MESSAGE_IGNORE_INSERTS |	\
			FORMAT_MESSAGE_ALLOCATE_BUFFER,		\
			NULL, \
			hr, \
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), \
			(LPTSTR) & output,	\
			0, \
			NULL);	\
		MessageBox(NULL, output, L"Error", MB_OK);	\
	}	\
}
#endif
#endif

//#define ReleaseCOM(x) {if(x){x->Release(); x=0;}}