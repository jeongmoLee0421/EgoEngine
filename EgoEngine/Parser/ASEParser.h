#pragma once

#include "../Inc/DX11Define.h"
#include "../Inc/ASEParsingData.h"

#include <vector>
#include <tuple>
#include <functional> // equal_to
#include <unordered_map>
#include <map>

// 2023 06 25 이정모 home

// 렉서가 의미있는 단위인 토큰을 던져주면
// 그 토큰에 맞게 데이터를 세팅

class ASELexer;
enum class EResourceType;

// 원래 ASEParser 내부에 선언될거였는데
// unordered_map을 멤버 변수로 넣으려면 인자로 hash class, equal_to class template 특수화를 먼저 선언해야 하고
// 그렇게 되면 hash, equal_to에 사용하는 타입은 이 class보다 먼저 선언해야 해서 여기까지 올라왔음
// 이럴 때 namespace를 써서 특정 공간 안의 것이라고 명시해주면 좋을 것 같다.
using VertexInfo = std::tuple<DirectX::XMFLOAT3, DirectX::XMFLOAT3, DirectX::XMFLOAT2>;

// 정점 중복 체크를 위한 hash table을 생성할 때 데이터를 묶기 위해 tuple을 사용했는데
// 사용자 정의 자료형을 사용하는 순간 기본으로 제공되는 hash function이 제거된다.
// 그래서 내가 직접 hash function을 정의함.
// 참고로 hash function 없이 key의 크기 비교가 이루어지는 이진 탐색 트리인 map의 경우는
// tuple<>로 묶어도 내부 데이터가 원시자료형이면 앞에서부터 순서대로 크기를 비교하기 때문에 문제 없다.
// 하지만 XMFLOAT3를 묶으면 대소 비교 연산자가 없기 때문에 자료형을 바꿔야함(XMFLOAT3는 DX에서 제공하는거라 내가 연산자 추가가 불가능할듯?)
// std 내부 hash class에 대한 완전 특수화
namespace std
{
	template<>
	class hash<VertexInfo>
	{
	public:
		std::uint64_t operator()(const VertexInfo& vertexInfo) const // 아마도 이 operator()를 부르는 객체는 const여서 멤버 변수를 수정하지 않겠다는 const 멤버 함수만 호출가능하기 때문에 뒤에 const
		{
			constexpr std::uint64_t prime = 0x100000001B3;
			std::uint64_t result = 0xcbf29ce484222325;

			DirectX::XMFLOAT3 position = std::get<0>(vertexInfo);
			DirectX::XMFLOAT3 normal = std::get<1>(vertexInfo);
			DirectX::XMFLOAT2 textureCoordinate = std::get<2>(vertexInfo);

			// 내가 직접 만든 union 타입이 뒤에 선언되어 있어도 여기서 인식이 가능하네?
			// 같은 class 내부이고 class 전체를 먼저 컴파일하고 BundleFloatAndInt의 구성요소를 확인한 다음에 operator() 함수의 지역변수 크기를 잡나본데?
			BundleFloatAndInt bundle(position);

			// 부동 소수점은 비트 연산이 불가하지만
			// union을 통해 int와 메모리를 공유해서 정수와 비트 연산을 수행했다.
			// 메모리 해석의 차이지 비트는 같으니까
			for (int i = 0; i < 3; ++i)
			{
				result = (result * prime) ^ (bundle.mInt3[i]);
			}

			bundle = normal;
			for (int i = 0; i < 3; ++i)
			{
				result = (result * prime) ^ (bundle.mInt3[i]);
			}

			bundle = textureCoordinate;
			for (int i = 0; i < 2; ++i)
			{
				result = (result * prime) ^ (bundle.mInt3[i]);
			}

			return result;
		}

		// XMFLOAT3가 부동소수점이다보니까 bit 연산이 불가함(bit 연산은 정수계열만 가능)
		// 그래서 union을 사용해서 int[3]이랑 메모리 공유하게 한뒤에 int로 비트연산 수행
		union BundleFloatAndInt
		{
			BundleFloatAndInt(DirectX::XMFLOAT3 float3)
				: mFloat3(float3)
			{}
			
			BundleFloatAndInt(DirectX::XMFLOAT2 float2)
				: mFloat3(float2.x, float2.y, 0.f)
			{}

			BundleFloatAndInt& operator=(DirectX::XMFLOAT3 float3)
			{
				this->mFloat3.x = float3.x;
				this->mFloat3.y = float3.y;
				this->mFloat3.z = float3.z;

				return *this;
			}

			BundleFloatAndInt& operator=(DirectX::XMFLOAT2 float2)
			{
				this->mFloat3.x = float2.x;
				this->mFloat3.y = float2.y;
				this->mFloat3.z = 0.f;

				return *this;
			}

			DirectX::XMFLOAT3 mFloat3;
			int mInt3[3];
		};
	};
}


// hash function 계산 후 인자로 들어온 key와 hash table에 들어있던 모든 key에 대해서 equal_to의 operator()가 호출되었다.
// 내 처음 생각은 hash value가 구해졌고 이것을 인덱스로하여(아마 hash value 숫자 크기가 너무 커서 내부적으로 추가적인 작업이 있을거임) 버킷으로 갔더니
// 이미 데이터가 존재하고 있다면 find()가 그 데이터에 대한 iterator를 반환한다고 생각했는데
// 이게 아니라 hash function 계산은 해두고서 key가 동일한 것이 있는지 비교하다가
// 만약 동일한 key가 있다면 이미 데이터가 있다는 거니까 앞서 계산한 hash_value를 통해서 데이터 찾아서 iterator를 반환해주는 것이고
// 동일한 key가 없다면 end()를 반환해주는 것 같다.
namespace std
{
	template <>
	struct equal_to<VertexInfo>
	{
		constexpr bool operator() (const VertexInfo& lhs, const VertexInfo& rhs) const
		{
			DirectX::XMFLOAT3 lPos = std::get<0>(lhs);
			DirectX::XMFLOAT3 rPos = std::get<0>(rhs);

			DirectX::XMFLOAT3 lNormal = std::get<1>(lhs);
			DirectX::XMFLOAT3 rNormal = std::get<1>(rhs);

			DirectX::XMFLOAT2 lTextureCoordinate = std::get<2>(lhs);
			DirectX::XMFLOAT2 rTextureCoordinate = std::get<2>(rhs);

			return (lPos.x == rPos.x)
				&& (lPos.y == rPos.y)
				&& (lPos.z == rPos.z)
				&& (lNormal.x == rNormal.x)
				&& (lNormal.y == rNormal.y)
				&& (lNormal.z == rNormal.z)
				&& (lTextureCoordinate.x == rTextureCoordinate.x)
				&& (lTextureCoordinate.y == rTextureCoordinate.y);
		}
	};
}


class ASEParser
{
public:
	ASEParser();
	~ASEParser();

public:
	bool OpenFile(const char* filePath);
	void ParsingAll();
	void OptimizeDataAll(ASE::Mesh* pMesh);

public:
	ASE::Mesh* GetMesh(int meshIndex);
	void SetVBResourceType(ASE::Mesh* pMesh, EResourceType eResourceType);
	void SetIBResourceType(ASE::Mesh* pMesh, EResourceType eResourceType);
	void SetIBSizeType(ASE::Mesh* pMesh, EResourceType eIBsizeType);

private:
	void OptimizeTVertex(ASE::Mesh* pMesh);
	void OptimizeTFace(ASE::Mesh* pMesh, std::map<std::tuple<float, float, float>, int>& textureCoordinateMap);
	void SaveOptimizeData(ASE::Mesh* pMesh);

private:
	void ParsingScene();
	void ParsingGeomObject();
	void ParsingNodeTM();
	void ParsingMesh();
	void ParsingMeshVertexList();
	void ParsingMeshFaceList();
	void ParsingMeshTVertList();
	void ParsingMeshTFaceList();
	void ParsingMeshNormals();

private:
	void CreateOneMeshAndAddList();
	void CreateOneVertexAndAddList();
	void CreateOneFaceAndAddList();
	void CreateOneTVertexAndAddList();

private:
	int ParsingNumberInt();
	const char* ParsingString();
	DirectX::XMFLOAT3 ParsingVector3(); // 3ds Max -> DX
	float ParsingNumberFloat();

public:
	static constexpr size_t TOKEN_MAX_LENGTH = 60;
	using VertexInfo = VertexInfo;

private:
	ASELexer* mLexer;
	unsigned short mToken;
	char mTokenStr[TOKEN_MAX_LENGTH];

private:
	int mAsciiExport;
	std::string mComment;
	ASE::Scene mScene;

	// mesh 구조체 하나를 채우기 위함
	ASE::Mesh* mOneMesh;
	// 하나의 오브젝트가 여러 mesh를 가지고 있음
	std::vector<ASE::Mesh*> mMeshList;

	// face를 이루는 vertex 정보
	ASE::Vertex* mOneVertex;

	// mesh를 이루는 face 정보
	ASE::Face* mOneFace;

	// 텍스처 좌표 정점
	ASE::TVertex* mOneTVertex;

	// 중복된 정점이 있나 확인하기 위한 hash table
	std::unordered_map<VertexInfo, int, std::hash<ASEParser::VertexInfo>, std::equal_to<ASEParser::VertexInfo>> mDuplicateVertexHashTable;
};