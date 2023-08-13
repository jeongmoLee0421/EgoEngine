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
	: mVertexIndex(-1) // 인덱스를 -1로 초기화해서 제대로 값 세팅이 안되면 인덱스 에러를 터트리기 위함
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
	// 포인터는 auto로 받아도 상관없지만 명확하게 포인터라고 나타내기 위함
	// container 원소의 자료형이 포인터가 아닐 때 auto로 사용하면 컴파일 에러가 없지만
	// auto*로 사용하면 컴파일러가 추론을 못해서
	// (컴파일러는 일반 변수로 추론하는데 프로그래머가 명시적으로 포인터 변수라고 적어두니까) 컴파일 에러를 띄워줌. 굳.
	// const auto&로도 물론 포인터를 받을 수 있지만 이 때는 포인터 앞이 아닌 변수 앞에 const가 붙여져서 역참조 후 값 대입이 가능해짐
	// const auto* const로 받아버리면 역참조 후 값 대입도 막고 변수에 메모리 주소 할당도 막을 수 있음
	
	// 소멸할 때는 불필요하게 다른 것들을 할 필요가 없으니까
	// 역참조로 데이터 수정 금지, 포인터 변수에 다른 메모리 주소 할당 금지
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

	// error, auto 추론 불가
	// indicesList는 내부 원소 자료형이 UINT인데 *로 추론하라고 프로그래머가 명시해서
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
