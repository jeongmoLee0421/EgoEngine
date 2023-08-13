#include "ASEParsingData.h"

ASE::Scene::Scene()
	: mFileName()
	, mFirstFrame(0)
	, mLastFrame(0)
	, mFrameSpeed(0)
	, mTicksPerFrame(0)
	, mBackgroundStatic()
	, mAmbientStatic()
{
}

ASE::Scene::~Scene()
{
}

ASE::Vertex::Vertex()
	: mVertexIndex(-1) // �ε����� -1�� �ʱ�ȭ�ؼ� ����� �� ������ �ȵǸ� �ε��� ������ ��Ʈ���� ����
	, mPosition()
	, mNormal()
	, mTextureCoordinate()
{
}

ASE::Vertex::~Vertex()
{
}

ASE::Face::Face()
	: mFaceIndex(-1)
	, mVertexIndices{-1, -1, -1}
	, mVertexNormal{ {},{},{} }
	, mMeshSmoothing(0)
	, mMeshMtlid(0)
	, mFaceNormal()
	, mTFace{-1,-1,-1}
{
}

ASE::Face::~Face()
{
}

ASE::Mesh::Mesh()
	: scene()
	, mNodeName()
	, mInheritPos()
	, mInheritRot()
	, mInheritScl()
	, mTMRow0()
	, mTMRow1()
	, mTMRow2()
	, mTMRow3()
	, mTMPos()
	, mTMRotAxis()
	, mTMRotAngle()
	, mTMScale()
	, mTMScaleAxis()
	, mTMScaleAxisAng()
	, mTimeValue(0)
	, mMeshNumVertex(0)
	, mMeshNumFaces(0)
	, mVertexList()
	, mFaceList()
	, mMeshNumTVertex(0)
	, mMeshNumTFaces(0)
	, mTVertexList()
	, mPropMotionBlur(0)
	, mPropCastShadow(0)
	, mPropRecvShadow(0)
	, mOptimizedVertexList()
	, mOptimizedIndicesList()
	, mVBResourceType()
{

}

ASE::Mesh::~Mesh()
{
	// �����ʹ� auto�� �޾Ƶ� ��������� ��Ȯ�ϰ� �����Ͷ�� ��Ÿ���� ����
	// container ������ �ڷ����� �����Ͱ� �ƴ� �� auto�� ����ϸ� ������ ������ ������
	// auto*�� ����ϸ� �����Ϸ��� �߷��� ���ؼ�
	// (�����Ϸ��� �Ϲ� ������ �߷��ϴµ� ���α׷��Ӱ� ��������� ������ ������� ����δϱ�) ������ ������ �����. ��.
	// const auto&�ε� ���� �����͸� ���� �� ������ �� ���� ������ ���� �ƴ� ���� �տ� const�� �ٿ����� ������ �� �� ������ ��������
	// const auto* const�� �޾ƹ����� ������ �� �� ���Ե� ���� ������ �޸� �ּ� �Ҵ絵 ���� �� ����
	
	// �Ҹ��� ���� ���ʿ��ϰ� �ٸ� �͵��� �� �ʿ䰡 �����ϱ�
	// �������� ������ ���� ����, ������ ������ �ٸ� �޸� �ּ� �Ҵ� ����
	for (const auto* const pVertex : mVertexList)
	{
		delete pVertex;
	}

	for (const auto* const pFace : mFaceList)
	{
		delete pFace;
	}

	for (const auto* const pTVertex : mTVertexList)
	{
		delete pTVertex;
	}

	// error, auto �߷� �Ұ�
	// indicesList�� ���� ���� �ڷ����� UINT�ε� *�� �߷��϶�� ���α׷��Ӱ� ����ؼ�
	//for (const auto* const pVertex : mOptimizedIndicesList);

	for (const auto* const pVertex : mOptimizedVertexList)
	{
		delete pVertex;
	}
}

ASE::TVertex::TVertex()
	: mTVertexIndex(-1)
	, mU(0.f)
	, mV(0.f)
	, mW(0.f)
{
}

ASE::TVertex::~TVertex()
{
}
