#pragma once

#include "../Inc/DX11Define.h"

#include <string>
#include <vector>

// 2023 06 27 ������ home

// ASE ������ �Ľ��� �����͸� ������ ����ü
// ��� ����� private ó���ϰ� ���� Get, Set ����ϴ� �� ��ȿ�����̶� ��������
// �׷��� struct�� ���� ��� public ó��

// Mesh���� resource type�� ������ �־ enum class�� ���漱�� �غ��Ҵ�.
// Mesh struct ���ο��� �����ͳ� ���� ������ �ƴԿ��� ���漱���� ������ ������
// enum class�� ����� ���� type�� ������ �����δ� ���� �ڷ����� ������ ���ΰ� ��� ������� �� �ʿ䰡 ���� ������ �ƴұ� �����غ���.
// ����� ���� type�̶�� ���ο� � ��������� �־ ũ�Ⱑ �󸶳� �Ǵ��� �˾ƾ������� enum class�� �����ϱ� 4����Ʈ�� �����Ű� mesh struct�� �������ϴµ� ������ ����.
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

		// ���� ����ȭ �Ŀ� ä���� normal �� texture coordinate
		DirectX::XMFLOAT3 mNormal;
		DirectX::XMFLOAT2 mTextureCoordinate; // �ϴ� u, v��
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

		// �ﰢ���� �̷�� ������ �ε���
		int mVertexIndices[3];
		
		// �ﰢ���� �̷�� ������ �븻
		DirectX::XMFLOAT3 mVertexNormal[3];

		int mMeshSmoothing;
		int mMeshMtlid;

		DirectX::XMFLOAT3 mFaceNormal;

		// face�� �̷�� texture vertex�� �ε���
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

		// ���� �ߺ��� ��� ������ ���� ���� �� �ε���
		std::vector<Vertex*> mOptimizedVertexList;
		std::vector<UINT> mOptimizedIndicesList;

		// �� �޽��� vb, ib, ibsize Ÿ��
		EResourceType mVBResourceType;
		EResourceType mIBResourceType;
		EResourceType mIBSizeType;
	};
};