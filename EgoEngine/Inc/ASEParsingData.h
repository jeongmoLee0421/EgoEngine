#pragma once

#include "../Inc/DX11Define.h"

#include <string>
#include <vector>

// 2023 06 27 이정모 home

// ASE 파일을 파싱한 데이터를 저장할 구조체
// 모든 멤버를 private 처리하고 전부 Get, Set 사용하는 건 비효율적이라 생각했음
// 그래서 struct로 만들어서 모두 public 처리

// Mesh에서 resource type을 가지고 있어서 enum class를 전방선언 해보았다.
// Mesh struct 내부에서 포인터나 참조 변수가 아님에도 전방선언이 가능한 이유는
// enum class가 사용자 정의 type은 맞지만 실제로는 원시 자료형인 정수라서 내부가 어떻게 생겼는지 알 필요가 없기 때문이 아닐까 생각해본다.
// 사용자 정의 type이라면 내부에 어떤 멤버변수가 있어서 크기가 얼마나 되는지 알아야하지만 enum class는 정수니까 4바이트로 잡을거고 mesh struct를 컴파일하는데 문제가 없다.
enum class EResourceType;

namespace ASE
{
	struct Scene
	{
		Scene();
		~Scene();

		std::string mFileName;
		int mFirstFrame;
		int mLastFrame;
		int mFrameSpeed;
		int mTicksPerFrame;
		DirectX::XMFLOAT3 mBackgroundStatic;
		DirectX::XMFLOAT3 mAmbientStatic;
	};

	struct Vertex
	{
		Vertex();
		~Vertex();

		int mVertexIndex;

		// local position
		DirectX::XMFLOAT3 mPosition;

		// 정점 최적화 후에 채워질 normal 및 texture coordinate
		DirectX::XMFLOAT3 mNormal;
		DirectX::XMFLOAT2 mTextureCoordinate; // 일단 u, v만
	};

	struct TVertex
	{
		TVertex();
		~TVertex();

		int mTVertexIndex;

		float mU;
		float mV;
		float mW;
	};

	struct Face
	{
		Face();
		~Face();

		int mFaceIndex;

		// 삼각형을 이루는 정점의 인덱스
		int mVertexIndices[3];
		
		// 삼각형을 이루는 정점의 노말
		DirectX::XMFLOAT3 mVertexNormal[3];

		int mMeshSmoothing;
		int mMeshMtlid;

		DirectX::XMFLOAT3 mFaceNormal;

		// face를 이루는 texture vertex의 인덱스
		int mTFace[3];
	};

	struct Mesh
	{
		Mesh();
		~Mesh();

		Scene scene;

		// node_tm
		std::string mNodeName;
		DirectX::XMFLOAT3 mInheritPos;
		DirectX::XMFLOAT3 mInheritRot;
		DirectX::XMFLOAT3 mInheritScl;
		DirectX::XMFLOAT3 mTMRow0;
		DirectX::XMFLOAT3 mTMRow1;
		DirectX::XMFLOAT3 mTMRow2;
		DirectX::XMFLOAT3 mTMRow3;
		DirectX::XMFLOAT3 mTMPos;
		DirectX::XMFLOAT3 mTMRotAxis;
		float mTMRotAngle;
		DirectX::XMFLOAT3 mTMScale;
		DirectX::XMFLOAT3 mTMScaleAxis;
		float mTMScaleAxisAng;

		// mesh
		int mTimeValue;
		int mMeshNumVertex;
		int mMeshNumFaces;

		std::vector<Vertex*> mVertexList;
		std::vector<Face*> mFaceList;

		int mMeshNumTVertex;
		int mMeshNumTFaces;
		
		std::vector<TVertex*> mTVertexList;

		int mPropMotionBlur;
		int mPropCastShadow;
		int mPropRecvShadow;

		// 정점 중복을 모두 제거한 최종 정점 및 인덱스
		std::vector<Vertex*> mOptimizedVertexList;
		std::vector<UINT> mOptimizedIndicesList;

		// 이 메시의 vb, ib, ibsize 타입
		EResourceType mVBResourceType;
		EResourceType mIBResourceType;
		EResourceType mIBSizeType;
	};
};