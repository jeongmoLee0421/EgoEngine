#pragma once

#include "../Inc/DX11Define.h"
#include "../Inc/ASEParsingData.h"

#include <vector>
#include <tuple>
#include <functional> // equal_to
#include <unordered_map>
#include <map>

// 2023 06 25 ������ home

// ������ �ǹ��ִ� ������ ��ū�� �����ָ�
// �� ��ū�� �°� �����͸� ����

class ASELexer;
enum class EResourceType;

// ���� ASEParser ���ο� ����ɰſ��µ�
// unordered_map�� ��� ������ �������� ���ڷ� hash class, equal_to class template Ư��ȭ�� ���� �����ؾ� �ϰ�
// �׷��� �Ǹ� hash, equal_to�� ����ϴ� Ÿ���� �� class���� ���� �����ؾ� �ؼ� ������� �ö����
// �̷� �� namespace�� �Ἥ Ư�� ���� ���� ���̶�� ������ָ� ���� �� ����.
using VertexInfo = std::tuple<DirectX::XMFLOAT3, DirectX::XMFLOAT3, DirectX::XMFLOAT2>;

// ���� �ߺ� üũ�� ���� hash table�� ������ �� �����͸� ���� ���� tuple�� ����ߴµ�
// ����� ���� �ڷ����� ����ϴ� ���� �⺻���� �����Ǵ� hash function�� ���ŵȴ�.
// �׷��� ���� ���� hash function�� ������.
// ����� hash function ���� key�� ũ�� �񱳰� �̷������ ���� Ž�� Ʈ���� map�� ����
// tuple<>�� ��� ���� �����Ͱ� �����ڷ����̸� �տ������� ������� ũ�⸦ ���ϱ� ������ ���� ����.
// ������ XMFLOAT3�� ������ ��� �� �����ڰ� ���� ������ �ڷ����� �ٲ����(XMFLOAT3�� DX���� �����ϴ°Ŷ� ���� ������ �߰��� �Ұ����ҵ�?)
// std ���� hash class�� ���� ���� Ư��ȭ
namespace std
{
	template<>
	class hash<VertexInfo>
	{
	public:
		std::uint64_t operator()(const VertexInfo& vertexInfo) const // �Ƹ��� �� operator()�� �θ��� ��ü�� const���� ��� ������ �������� �ʰڴٴ� const ��� �Լ��� ȣ�Ⱑ���ϱ� ������ �ڿ� const
		{
			constexpr std::uint64_t prime = 0x100000001B3;
			std::uint64_t result = 0xcbf29ce484222325;

			DirectX::XMFLOAT3 position = std::get<0>(vertexInfo);
			DirectX::XMFLOAT3 normal = std::get<1>(vertexInfo);
			DirectX::XMFLOAT2 textureCoordinate = std::get<2>(vertexInfo);

			// ���� ���� ���� union Ÿ���� �ڿ� ����Ǿ� �־ ���⼭ �ν��� �����ϳ�?
			// ���� class �����̰� class ��ü�� ���� �������ϰ� BundleFloatAndInt�� ������Ҹ� Ȯ���� ������ operator() �Լ��� �������� ũ�⸦ �⳪����?
			BundleFloatAndInt bundle(position);

			// �ε� �Ҽ����� ��Ʈ ������ �Ұ�������
			// union�� ���� int�� �޸𸮸� �����ؼ� ������ ��Ʈ ������ �����ߴ�.
			// �޸� �ؼ��� ������ ��Ʈ�� �����ϱ�
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

		// XMFLOAT3�� �ε��Ҽ����̴ٺ��ϱ� bit ������ �Ұ���(bit ������ �����迭�� ����)
		// �׷��� union�� ����ؼ� int[3]�̶� �޸� �����ϰ� �ѵڿ� int�� ��Ʈ���� ����
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


// hash function ��� �� ���ڷ� ���� key�� hash table�� ����ִ� ��� key�� ���ؼ� equal_to�� operator()�� ȣ��Ǿ���.
// �� ó�� ������ hash value�� �������� �̰��� �ε������Ͽ�(�Ƹ� hash value ���� ũ�Ⱑ �ʹ� Ŀ�� ���������� �߰����� �۾��� ��������) ��Ŷ���� ������
// �̹� �����Ͱ� �����ϰ� �ִٸ� find()�� �� �����Ϳ� ���� iterator�� ��ȯ�Ѵٰ� �����ߴµ�
// �̰� �ƴ϶� hash function ����� �صΰ� key�� ������ ���� �ִ��� ���ϴٰ�
// ���� ������ key�� �ִٸ� �̹� �����Ͱ� �ִٴ� �Ŵϱ� �ռ� ����� hash_value�� ���ؼ� ������ ã�Ƽ� iterator�� ��ȯ���ִ� ���̰�
// ������ key�� ���ٸ� end()�� ��ȯ���ִ� �� ����.
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

	// mesh ����ü �ϳ��� ä��� ����
	ASE::Mesh* mOneMesh;
	// �ϳ��� ������Ʈ�� ���� mesh�� ������ ����
	std::vector<ASE::Mesh*> mMeshList;

	// face�� �̷�� vertex ����
	ASE::Vertex* mOneVertex;

	// mesh�� �̷�� face ����
	ASE::Face* mOneFace;

	// �ؽ�ó ��ǥ ����
	ASE::TVertex* mOneTVertex;

	// �ߺ��� ������ �ֳ� Ȯ���ϱ� ���� hash table
	std::unordered_map<VertexInfo, int, std::hash<ASEParser::VertexInfo>, std::equal_to<ASEParser::VertexInfo>> mDuplicateVertexHashTable;
};