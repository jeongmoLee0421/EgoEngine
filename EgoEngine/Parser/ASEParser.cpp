#include "ASEParser.h"
#include "../Inc/ASELexer.h"
#include "../Inc/ASEParsingData.h"

#include <cassert>
#include <unordered_map>
#include <utility>
#include <map>
#include <tuple>

enum EASEToken : unsigned short
{
	TOKENR_3DSMAX_ASCIIEXPORT = 0,
	TOKENR_COMMENT = 1,
	TOKENR_SCENE = 2,
	TOKENR_MATERIAL_LIST = 3,
	TOKENR_MATERIAL = 4,
	TOKENR_LIGHTOBJECT = 5,
	TOKENR_GEOMOBJECT = 6,
	TOKENR_NODE_NAME = 7,
	TOKENR_NODE_TM = 8,
	TOKENR_MESH = 9,
	TOKENR_MESH_VERTEX = 10,
	TOKENR_MESH_FACE_LIST = 11,
	TOKENR_MESH_FACE = 12,
	TOKENR_MESH_NUMTVERTEX = 13,
	TOKENR_MESH_NUMCVERTEX = 14,
	TOKENR_MESH_NORMALS = 15,
	TOKENR_MESH_FACENORMAL = 16,
	TOKENR_MESH_VERTEXNORMAL = 17,
	TOKENR_HELPEROBJECT = 18,
	TOKENR_NODE_PARENT = 19,
	TOKENR_HELPER_CLASS = 20,
	TOKENR_INHERIT_POS = 21,
	TOKENR_INHERIT_ROT = 22,
	TOKENR_INHERIT_SCL = 23,
	TOKENR_TM_ROW0 = 24,
	TOKENR_TM_ROW1 = 25,
	TOKENR_TM_ROW2 = 26,
	TOKENR_TM_ROW3 = 27,
	TOKENR_TM_POS = 28,
	TOKENR_TM_ROTAXIS = 29,
	TOKENR_TM_ROTANGLE = 30,
	TOKENR_TM_SCALE = 31,
	TOKENR_TM_SCALEAXIS = 32,
	TOKENR_TM_SCALEAXISANG = 33,
	TOKENR_BOUNDINGBOX_MIN = 34,
	TOKENR_BOUNDINGBOX_MAX = 35,
	TOKENR_TIMEVALUE = 36,
	TOKENR_MESH_NUMVERTEX = 37,
	TOKENR_MESH_NUMFACES = 38,
	TOKENR_GROUP = 39,
	TOKENR_MESH_TVERT = 40,
	TOKENR_MESH_TFACELIST = 41,
	TOKENR_MESH_TVERTLIST = 42,
	TOKENR_PROP_MOTIONBLUR = 43,
	TOKENR_PROP_CASTSHADOW = 44,
	TOKENR_PROP_RECVSHADOW = 45,
	TOKENR_MATERIAL_REF = 46,
	TOKENR_SCENE_FILENAME = 48,
	TOKENR_SCENE_FIRSTFRAME = 49,
	TOKENR_SCENE_LASTFRAME = 50,
	TOKENR_SCENE_FRAMESPEED = 51,
	TOKENR_SCENE_TICKSPERFRAME = 52,
	TOKENR_SCENE_ENVMAP = 53,
	TOKENR_MAP_NAME = 54,
	TOKENR_MAP_CLASS = 55,
	TOKENR_MAP_SUBNO = 56,
	TOKENR_MAP_AMOUNT = 57,
	TOKENR_SCENE_AMBIENT_STATIC = 58,
	TOKENR_MATERIAL_COUNT = 59,
	TOKENR_MATERIAL_NAME = 60,
	TOKENR_MATERIAL_CLASS = 61,
	TOKENR_MATERIAL_AMBIENT = 62,
	TOKENR_MATERIAL_DIFFUSE = 63,
	TOKENR_MATERIAL_SPECULAR = 64,
	TOKENR_MATERIAL_SHINE = 65,
	TOKENR_MATERIAL_SHINESTRENGTH = 66,
	TOKENR_MATERIAL_TRANSPARENCY = 67,
	TOKENR_MATERIAL_WIRESIZE = 68,
	TOKENR_MATERIAL_SHADING = 69,
	TOKENR_MATERIAL_XP_FALLOFF = 70,
	TOKENR_MATERIAL_SELFILLUM = 71,
	TOKENR_MATERIAL_TWOSIDED = 72,
	TOKENR_MATERIAL_FALLOFF = 73,
	TOKENR_MATERIAL_XP_TYPE = 74,
	TOKENR_MAP_DIFFUSE = 75,
	TOKENR_MESH_VERTEX_LIST = 76,
	TOKENR_MESH_NUMTVFACES = 77,
	TOKENR_MESH_CVERTLIST = 78,
	TOKENR_LIGHT_TYPE = 79,
	TOKENR_LIGHT_SHADOWS = 80,
	TOKENR_LIGHT_USELIGHT = 81,
	TOKENR_LIGHT_SPOTSHAPE = 82,
	TOKENR_LIGHT_USEGLOBAL = 83,
	TOKENR_LIGHT_ABSMAPBIAS = 84,
	TOKENR_LIGHT_OVERSHOOT = 85,
	TOKENR_LIGHT_SETTINGS = 86,
	TOKENR_LIGHT_COLOR = 87,
	TOKENR_LIGHT_INTENS = 88,
	TOKENR_LIGHT_ASPECT = 89,
	TOKENR_LIGHT_TDIST = 90,
	TOKENR_LIGHT_MAPBIAS = 91,
	TOKENR_LIGHT_MAPRANGE = 92,
	TOKENR_LIGHT_MAPSIZE = 93,
	TOKENR_LIGHT_RAYBIAS = 94,
	TOKENR_MESH_SMOOTHING = 95,
	TOKENR_MESH_MTLID = 96,
	TOKENR_WIREFRAME_COLOR = 97,
	TOKENR_TM_ANIMATION = 98,
	TOKENR_CONTROL_POS_TRACK = 99,
	TOKENR_CONTROL_ROT_TRACK = 100,
	TOKENR_CONTROL_POS_SAMPLE = 101,
	TOKENR_CONTROL_ROT_SAMPLE = 102,
	TOKENR_CAMERAOBJECT = 103,
	TOKENR_CAMERA_TYPE = 104,
	TOKENR_CAMERA_SETTINGS = 105,
	TOKENR_CAMERA_NEAR = 106,
	TOKENR_CAMERA_FAR = 107,
	TOKENR_CAMERA_FOV = 108,
	TOKENR_CAMERA_TDIST = 109,
	TOKENR_NUMSUBMTLS = 110,
	TOKENR_SUBMATERIAL = 111,
	TOKENR_MAP_SPECULAR = 112,
	TOKENR_MAP_SHINE = 113,
	TOKENR_MAP_GENERIC = 114,
	TOKENR_BITMAP = 115,
	TOKENR_MAP_TYPE = 116,
	TOKENR_UVW_U_OFFSET = 117,
	TOKENR_UVW_V_OFFSET = 118,
	TOKENR_UVW_U_TILING = 119,
	TOKENR_UVW_V_TILING = 120,
	TOKENR_UVW_ANGLE = 121,
	TOKENR_UVW_BLUR = 122,
	TOKENR_UVW_BLUR_OFFSET = 123,
	TOKENR_UVW_NOUSE_AMT = 124,
	TOKENR_UVW_NOISE_SIZE = 125,
	TOKENR_UVW_NOISE_LEVEL = 126,
	TOKENR_UVW_NOISE_PHASE = 127,
	TOKENR_BITMAP_FILTER = 128,
	TOKENR_MESH_MAPPINGCHANNEL = 129,
	TOKENR_MESH_TFACE = 130,
	TOKENR_CONTROL_POS_BEZIER = 131,
	TOKENR_CONTROL_BEZIER_POS_KEY = 132,
	TOKENR_CONTROL_ROT_TCB = 133,
	TOKENR_CONTROL_TCB_ROT_KEY = 134,
	TOKENR_MAP_OPACITY = 135,
	TOKENR_MATERIAL_FACEMAP = 136,
	TOKENR_MESH_NUMSKINWEIGHT = 137,
	TOKENR_MESH_NUMBONE = 138,
	TOKENR_BONE_LIST = 139,
	TOKENR_BONE_NAME = 140,
	TOKENR_MESH_WVERTEXS = 141,
	TOKENR_MESH_WEIGHT = 142,
	TOKENR_BONE_BLENGING_WEIGHT = 143,
	TOKENR_MAP_REFLECT = 144,
	TOKENR_MAP_REFRACT = 145,
	TOKENR_MAP_BUMP = 146,
	TOKENR_SCENE_BACKGROUND_STATIC = 147,
	TOKENR_MAP_SELFILLUM = 148,
	TOKENR_LIGHT_EXCLUDELIST = 149,
	TOKENR_LIGHT_NUMEXCLUDED = 150,
	TOKENR_LIGHT_EXCLUDED_INCLUDE = 151,
	TOKENR_LIGHT_EXCLUDED_AFFECT_ILLUM = 152,
	TOKENR_LIGHT_EXCLUDED_AFFECT_SHADOW = 153,
	TOKENR_LIGHT_EXCLUDED = 154,
	TOKENR_LIGHT_HOTSPOT = 155,
	TOKENR_LIGHT_FALLOFF = 156,
	TOKENR_MESH_FACEMAPLIST = 157,
	TOKENR_MESH_FACEMAP = 158,
	TOKENR_MESH_FACEMAPVERT = 159,
	TOKENR_BITMAP_INVERT = 160,
	TOKENR_SHAPEOBJECT = 161,
	TOKENR_SHAPE_LINECOUNT = 162,
	TOKENR_SHAPE_LINE = 163,
	TOKENR_SHAPE_VERTEXCOUNT = 164,
	TOKENR_SHAPE_VERTEX_KNOT = 165,
	TOKENR_SHAPE_VERTEX_INTERP = 166,
	TOKENR_SHAPE_CLOSED = 167,
	TOKENR_IK_JOINT = 168,
	TOKENR_IK_TYPE = 169,
	TOKENR_IK_DOF = 170,
	TOKENR_IK_XACTIVE = 171,
	TOKENR_IK_YACTIVE = 172,
	TOKENR_IK_ZACTIVE = 173,
	TOKENR_IK_XLIMITED = 174,
	TOKENR_IK_YLIMITED = 175,
	TOKENR_IK_ZLIMITED = 176,
	TOKENR_IK_XEASE = 177,
	TOKENR_IK_YEASE = 178,
	TOKENR_IK_ZEASE = 179,
	TOKENR_IK_LIMITEXACT = 180,
	TOKENR_IK_JOINTINFO = 181,
	TOKENR_LIGHT_ATTNSTART = 182,
	TOKENR_LIGHT_ATTNEND = 183,
	TOKENR_MAP_AMBIENT = 184,
	TOKENR_MESH_VERTCOL = 185,
	TOKENR_MESH_NUMCVFACES = 186,
	TOKENR_MESH_CFACELIST = 187,
	TOKENR_MESH_CFACE = 188,
	TOKENR_MAP_SHINESTRENGTH = 189,
	TOKENR_MAP_FILTERCOLOR = 190,

	TOKENR_NODE_VISIBILITY_TRACK = 191,
	TOKENR_CONTROL_FLOAT_SAMPLE = 192,
	TOKENR_CONTROL_FLOAT_KEY = 193,

	TOKENR_BONE_PROPERTY = 194,
	TOKENR_BONE = 195,
	TOKENR_SKIN_INITTM = 196,
	TOKENR_CONTROL_SCALE_TRACK = 197,
	TOKENR_CONTROL_SCALE_SAMPLE = 198,
	TOKENR_SCENE_AMBIENT_ANIM = 199,
	TOKENR_CONTROL_POINT3_KEY = 200,
	TOKENR_CONTROL_TCB_POINT3_KEY = 201,
	TOKENR_CONTROL_TCB_FLOAT_KEY = 202,
	TOKENR_CONTROL_TCB_POS_KEY = 203,
	TOKENR_CONTROL_TCB_SCALE_KEY = 204,
	TOKENR_CONTROL_BEZIER_FLOAT_KEY = 205,
	TOKENR_CONTROL_BEZIER_POINT3_KEY = 206,
	TOKENR_CONTROL_BEZIER_SCALE_KEY = 207,
	TOKENR_CONTROL_POS_LINEAR = 208,
	TOKENR_CONTROL_POS_TCB = 209,
	TOKENR_CONTROL_ROT_LINEAR = 210,
	TOKENR_CONTROL_ROT_BEZIER = 211,
	TOKENR_CONTROL_SCALE_LINEAR = 212,
	TOKENR_CONTROL_SCALE_TCB = 213,
	TOKENR_CONTROL_SCALE_BEZIER = 214,

	TOKENR_CONTROL_POS_KEY = 215,
	TOKENR_CONTROL_ROT_KEY = 216,
	TOKENR_CONTROL_SCALE_KEY = 217,

	TOKENR_CONTROL_POINT3_TCB = 218,
	TOKENR_CONTROL_POINT3_BEZIER = 219,
	TOKENR_CONTROL_COLOR_BEZIER = 220,
	TOKENR_CONTROL_POINT3_SAMPLE = 221,

	TOKENR_CONTROL_FLOAT_TCB = 222,
	TOKENR_CONTROL_FLOAT_BEZIER = 223,
	TOKENR_CONTROL_FLOAT_LINEAR = 224,

	TOKENR_CAMERA_ANIMATION = 225,
	TOKENR_LIGHT_ANIMATION = 226,

	TOKENR_MATERIAL_WIREUNITS = 227,

	TOKENR_SCENE_MESHFRAMESTEP = 228,
	TOKENR_SCENE_KEYFRAMESTEP = 229,

	TOKENR_PROP_HIDDEN = 230,
	TOKENR_PROP_NORENDERABLE = 231,
	TOKENR_PROP_BONE = 232
};

ASEParser::ASEParser()
	: mLexer(nullptr)
	, mToken(0)
	, mOneMesh(nullptr)
	, mMeshList()
	, mOneVertex(nullptr)
	, mOneFace(nullptr)
	, mOneTVertex(nullptr)
	, mDuplicateVertexHashTable()
{
	mLexer = new ASELexer();

	memset(mTokenStr, 0, sizeof(mTokenStr));
}

ASEParser::~ASEParser()
{
	// �ļ��� �Ҹ��ϸ� �ε��ߴ� ���� ��� mesh�� delete
	for (const auto* const pMesh : mMeshList)
	{
		delete pMesh; // mesh�� delete �ϸ� ���ο� �����Ҵ��� vertex �� face�� �����
	}

	delete mLexer;

	mDuplicateVertexHashTable.clear();
}

bool ASEParser::OpenFile(const char* filePath)
{
	bool bSuccess = mLexer->OpenFile(filePath);

	if (bSuccess)
		return true;
	else return false;
}

void ASEParser::ParsingAll()
{
	while (true)
	{
		mToken = mLexer->GetToken(mTokenStr);

		switch (mToken)
		{
		case ASELexer::TOKEN_END:
			return;

			// ��ͱ����� �ۼ����� �ʰ� �Ľ��ؾ��ϴ� �����鸶�� ���ο� �Լ��� ȣ���ϱ� ������ ���� �ٱ� ���������� �߰�ȣ�� ũ�� �ǹ� ����
		case ASELexer::TOKEN_OPEN_BRACE:
		case ASELexer::TOKEN_CLOSE_BRACE:
			break;

		case EASEToken::TOKENR_3DSMAX_ASCIIEXPORT:
			mAsciiExport = ParsingNumberInt();
			break;

		case EASEToken::TOKENR_COMMENT:
			mComment = std::string(ParsingString());
			break;

		case EASEToken::TOKENR_SCENE:
			ParsingScene();
			break;

		case EASEToken::TOKENR_GEOMOBJECT:
			CreateOneMeshAndAddList();
			ParsingGeomObject();
			break;
		}
	}
}

// ���� ����ȭ�� �� �� �ϴ� �ߺ� ���� ������� ���� ��� �����͸� �� �޾Ƴ��� ���ϴ°� ����
// �� �޾Ƴ��� ���� �ϳ��ϳ� �����鼭 �������� �ʰ�
// �޴� ���ÿ� �ߺ����� ����ؼ� ������� �ϸ� �ڿ� ������ ���� position, normal, texture coordinate�� ������� �ʾƼ� ��������.
void ASEParser::OptimizeDataAll(ASE::Mesh* pMesh)
{
	OptimizeTVertex(pMesh);

	// ���ο� ������ ���鶧 �ε����� �����ֱ� ����
	int newVertexIndex = 0;

	for (int i = 0; i < pMesh->mFaceList.size(); ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			int vertexIndex = pMesh->mFaceList[i]->mVertexIndices[j];

			DirectX::XMFLOAT3 vertexPosition = pMesh->mVertexList[vertexIndex]->mPosition;
			DirectX::XMFLOAT3 vertexNormal = pMesh->mFaceList[i]->mVertexNormal[j];

			int textureVertexIndex = pMesh->mFaceList[i]->mTFace[j];
			DirectX::XMFLOAT2 vertexTextureCoordinate(pMesh->mTVertexList[textureVertexIndex]->mU, pMesh->mTVertexList[textureVertexIndex]->mV);

			auto vertexInfo = std::make_tuple(vertexPosition, vertexNormal, vertexTextureCoordinate);

			// position, normal, texture coordinate ��ΰ� ��ġ�ϴ� ������ �ִ��� �ߺ� üũ
			auto it = mDuplicateVertexHashTable.find(vertexInfo);

			// ��ġ�ϴ°� ���ٸ� ���Ӱ� �����ؾ��ϴ� �����̱� ������
			// ���ο� �ε����� �ο��ϸ鼭 hash table�� �־��ְ� ���Ӱ� �ο��� �ε����� face�� �̷�� ���� index ����
			if (it == mDuplicateVertexHashTable.end())
			{
				auto newVertexInfo = std::make_pair(vertexInfo, newVertexIndex);
				mDuplicateVertexHashTable.insert(newVertexInfo);
				pMesh->mFaceList[i]->mVertexIndices[j] = newVertexIndex;

				++newVertexIndex;
			}
			// ��ġ�ϴ°� �ִٸ� ���� vertexIndex ��ſ� hash table�� �����ϴ� index�� ����
			else
			{
				pMesh->mFaceList[i]->mVertexIndices[j] = it->second;
			}
		}
	}

	SaveOptimizeData(pMesh);
}

void ASEParser::ParsingScene()
{
	while (true)
	{
		mToken = mLexer->GetToken(mTokenStr);

		switch (mToken)
		{
			// ASE ��ū�� ������ ���ο� ������ ���Դٸ� ������ ����� �ݴ� �߰�ȣ�� ������ ����
		case ASELexer::TOKEN_CLOSE_BRACE:
			return;

		case EASEToken::TOKENR_SCENE_FILENAME:
			mScene.mFileName = std::string(ParsingString());
			break;

		case EASEToken::TOKENR_SCENE_FIRSTFRAME:
			mScene.mFirstFrame = ParsingNumberInt();
			break;

		case EASEToken::TOKENR_SCENE_LASTFRAME:
			mScene.mLastFrame = ParsingNumberInt();
			break;

		case EASEToken::TOKENR_SCENE_FRAMESPEED:
			mScene.mFrameSpeed = ParsingNumberInt();
			break;

		case EASEToken::TOKENR_SCENE_TICKSPERFRAME:
			mScene.mTicksPerFrame = ParsingNumberInt();
			break;

		case EASEToken::TOKENR_SCENE_BACKGROUND_STATIC:
			mScene.mBackgroundStatic = ParsingVector3();
			break;

		case EASEToken::TOKENR_SCENE_AMBIENT_STATIC:
			mScene.mAmbientStatic = ParsingVector3();
			break;
		}
	}
}

void ASEParser::ParsingGeomObject()
{
	while (true)
	{
		mToken = mLexer->GetToken(mTokenStr);

		switch (mToken)
		{
		case ASELexer::TOKEN_CLOSE_BRACE:
			return;

		case EASEToken::TOKENR_NODE_NAME:
			mOneMesh->mNodeName = std::string(ParsingString());
			break;

		case EASEToken::TOKENR_NODE_TM:
			ParsingNodeTM();
			break;

		case EASEToken::TOKENR_MESH:
			ParsingMesh();
			break;

		case EASEToken::TOKENR_PROP_MOTIONBLUR:
			mOneMesh->mPropMotionBlur = ParsingNumberInt();
			break;

		case EASEToken::TOKENR_PROP_CASTSHADOW:
			mOneMesh->mPropCastShadow = ParsingNumberInt();
			break;

		case EASEToken::TOKENR_PROP_RECVSHADOW:
			mOneMesh->mPropRecvShadow = ParsingNumberInt();
			break;
		}
	}
}

void ASEParser::ParsingNodeTM()
{
	while (true)
	{
		mToken = mLexer->GetToken(mTokenStr);

		switch (mToken)
		{
		case ASELexer::TOKEN_CLOSE_BRACE:
			return;

		case EASEToken::TOKENR_NODE_NAME:
			mOneMesh->mNodeName = std::string(ParsingString());
			break;

		case EASEToken::TOKENR_INHERIT_POS:
			mOneMesh->mInheritPos = ParsingVector3();
			break;

		case EASEToken::TOKENR_INHERIT_ROT:
			mOneMesh->mInheritRot = ParsingVector3();
			break;

		case EASEToken::TOKENR_INHERIT_SCL:
			mOneMesh->mInheritScl = ParsingVector3();
			break;

		case EASEToken::TOKENR_TM_ROW0:
			mOneMesh->mTMRow0 = ParsingVector3();
			break;

			// DX�� Max�� ��ǥ�谡 �޶� y����, z������ ��ȯ�ؾ���
		case EASEToken::TOKENR_TM_ROW1:
			mOneMesh->mTMRow2 = ParsingVector3();
			break;

		case EASEToken::TOKENR_TM_ROW2:
			mOneMesh->mTMRow1 = ParsingVector3();
			break;

		case EASEToken::TOKENR_TM_ROW3:
			mOneMesh->mTMRow3 = ParsingVector3();
			break;

		case EASEToken::TOKENR_TM_POS:
			mOneMesh->mTMPos = ParsingVector3();
			break;

		case EASEToken::TOKENR_TM_ROTAXIS:
			mOneMesh->mTMRotAxis = ParsingVector3();
			break;

		case EASEToken::TOKENR_TM_ROTANGLE:
			mOneMesh->mTMRotAngle = ParsingNumberFloat();
			break;

		case EASEToken::TOKENR_TM_SCALE:
			mOneMesh->mTMScale = ParsingVector3();
			break;

		case EASEToken::TOKENR_TM_SCALEAXIS:
			mOneMesh->mTMScaleAxis = ParsingVector3();
			break;

		case EASEToken::TOKENR_TM_SCALEAXISANG:
			mOneMesh->mTMScaleAxisAng = ParsingNumberFloat();
			break;
		}
	}
}

void ASEParser::ParsingMesh()
{
	while (true)
	{
		mToken = mLexer->GetToken(mTokenStr);

		switch (mToken)
		{
		case ASELexer::TOKEN_CLOSE_BRACE:
			return;

		case EASEToken::TOKENR_TIMEVALUE:
			mOneMesh->mTimeValue = ParsingNumberInt();
			break;

		case EASEToken::TOKENR_MESH_NUMVERTEX:
			mOneMesh->mMeshNumVertex = ParsingNumberInt();
			break;

		case EASEToken::TOKENR_MESH_NUMFACES:
			mOneMesh->mMeshNumFaces = ParsingNumberInt();
			break;

		case EASEToken::TOKENR_MESH_VERTEX_LIST:
			ParsingMeshVertexList();
			break;

		case EASEToken::TOKENR_MESH_FACE_LIST:
			ParsingMeshFaceList();
			break;

		case EASEToken::TOKENR_MESH_NUMTVERTEX:
			mOneMesh->mMeshNumTVertex = ParsingNumberInt();
			break;

		case EASEToken::TOKENR_MESH_TVERTLIST:
			ParsingMeshTVertList();
			break;

		case EASEToken::TOKENR_MESH_NUMTVFACES:
			mOneMesh->mMeshNumTFaces = ParsingNumberInt();
			break;

		case EASEToken::TOKENR_MESH_TFACELIST:
			ParsingMeshTFaceList();
			break;

		case EASEToken::TOKENR_MESH_NORMALS:
			ParsingMeshNormals();
			break;
		}
	}
}

void ASEParser::ParsingMeshVertexList()
{
	while (true)
	{
		mToken = mLexer->GetToken(mTokenStr);

		switch (mToken)
		{
		case ASELexer::TOKEN_CLOSE_BRACE:
			return;

		case EASEToken::TOKENR_MESH_VERTEX:
			CreateOneVertexAndAddList();
			mOneVertex->mVertexIndex = ParsingNumberInt();
			mOneVertex->mPosition = ParsingVector3();
			break;
		}
	}
}

void ASEParser::ParsingMeshFaceList()
{
	while (true)
	{
		mToken = mLexer->GetToken(mTokenStr);

		switch (mToken)
		{
		case ASELexer::TOKEN_CLOSE_BRACE:
			return;

		case EASEToken::TOKENR_MESH_FACE:
			CreateOneFaceAndAddList();
			mOneFace->mFaceIndex = ParsingNumberInt();

			// max�� ccw�� �ﰢ���� �����ϴµ� DX�� cw�� �ﰢ���� �����ϱ� ������ 2, 3�� ���� �ε��� ��ȯ �ʿ�
			// ���� ����ȭ �ܰ迡�� �� ����
			mOneFace->mVertexIndices[0] = ParsingNumberInt();
			mOneFace->mVertexIndices[1] = ParsingNumberInt();
			mOneFace->mVertexIndices[2] = ParsingNumberInt();
			break;

		case EASEToken::TOKENR_MESH_SMOOTHING:
			mOneFace->mMeshSmoothing = ParsingNumberInt();
			break;

		case EASEToken::TOKENR_MESH_MTLID:
			mOneFace->mMeshMtlid = ParsingNumberInt();
			break;
		}
	}
}

void ASEParser::ParsingMeshTVertList()
{
	while (true)
	{
		mToken = mLexer->GetToken(mTokenStr);

		switch (mToken)
		{
		case ASELexer::TOKEN_CLOSE_BRACE:
			return;

		case EASEToken::TOKENR_MESH_TVERT:
			CreateOneTVertexAndAddList();
			mOneTVertex->mTVertexIndex = ParsingNumberInt();
			mOneTVertex->mU = ParsingNumberFloat();
			mOneTVertex->mV = 1 - ParsingNumberFloat(); // dx�� �ؽ�ó ��ǥ��� �»���� (0,0) max�� ���ϴ��� (0,0)�̱� ������ v���� 1���� �������
			mOneTVertex->mW = ParsingNumberFloat(); // w�� ���� ������� ������ Ȯ�强�� ���� ������
			break;
		}
	}
}

void ASEParser::ParsingMeshTFaceList()
{
	while (true)
	{
		mToken = mLexer->GetToken(mTokenStr);

		switch (mToken)
		{
		case ASELexer::TOKEN_CLOSE_BRACE:
			return;

		case EASEToken::TOKENR_MESH_TFACE:
			int faceIndex = ParsingNumberInt();
			mOneMesh->mFaceList[faceIndex]->mTFace[0] = ParsingNumberInt();
			mOneMesh->mFaceList[faceIndex]->mTFace[1] = ParsingNumberInt();
			mOneMesh->mFaceList[faceIndex]->mTFace[2] = ParsingNumberInt();
			break;
		}
	}
}

void ASEParser::ParsingMeshNormals()
{
	// face�̷�� 3���� vertex normal�� ������ face�� �����ϱ� ����
	int faceIndex = 0;

	// �ϳ��� face�� 0,1,2 �ε����� �����ϱ� ����
	int vertexIndex = 0;

	while (true)
	{
		mToken = mLexer->GetToken(mTokenStr);

		switch (mToken)
		{
		case ASELexer::TOKEN_CLOSE_BRACE:
			return;

		case EASEToken::TOKENR_MESH_FACENORMAL:
		{
			faceIndex = ParsingNumberInt();
			vertexIndex = 0;

			mOneMesh->mFaceList[faceIndex]->mFaceNormal = ParsingVector3();
		}
		break;

		case EASEToken::TOKENR_MESH_VERTEXNORMAL:
		{
			mOneMesh->mFaceList[faceIndex]->mVertexIndices[vertexIndex] = ParsingNumberInt();
			mOneMesh->mFaceList[faceIndex]->mVertexNormal[vertexIndex] = ParsingVector3();

			++vertexIndex;
		}
		break;
		}
	}
}

ASE::Mesh* ASEParser::GetMesh(int meshIndex)
{
	return mMeshList[meshIndex];
}

void ASEParser::SetVBResourceType(ASE::Mesh* pMesh, EResourceType eResourceType)
{
	pMesh->mVBResourceType = eResourceType;
}

void ASEParser::SetIBResourceType(ASE::Mesh* pMesh, EResourceType eResourceType)
{
	pMesh->mIBResourceType = eResourceType;
}

void ASEParser::SetIBSizeType(ASE::Mesh* pMesh, EResourceType eIBsizeType)
{
	pMesh->mIBSizeType = eIBsizeType;
}

void ASEParser::OptimizeTVertex(ASE::Mesh* pMesh)
{
	// unordered_map�� ���� �ڷ����� float������ tuple�� ���� ������ �⺻ hash function�� �����ϱ� ������ ���� �����ؾ��Ѵ�.
	// ������ map�� ���� �ڷ����� ���� tuple�� ��� ��Һ񱳰� �����ϱ� ������ ���� ����, ���ʺ��� float ���� ���س���(��ȸ�� ���� ���� ��ȸ(��, ��, ��))
	// map�� ����Ž��Ʈ���� key�� ũ�� ��(<, >)�� �����ϸ� �ȴ�. ����� XMFLOAT3���� ��Һ񱳰� ��� map�� �̰� ���� ����
	std::map<std::tuple<float, float, float>, int> textureCoordinateMap;

	for (int i = 0; i < pMesh->mTVertexList.size(); ++i)
	{
		int texVertexIndex = pMesh->mTVertexList[i]->mTVertexIndex;
		float u = pMesh->mTVertexList[i]->mU;
		float v = pMesh->mTVertexList[i]->mV;
		float w = pMesh->mTVertexList[i]->mW;

		// u, v, w�� �޸� �ּҸ� ���� lvalue��. make_tuple<>()�� ��� �Լ� �Ű����� Ÿ���� _Types&&...��. ���� ������ �ް� ����
		// ���������� lvalue�� �Ѿ�� �Լ� �Ű����� Ÿ�԰� ���ø� �Ű����� Ÿ���� ��� lvalue ������ �߷е�
		// �׷��� ���ø� �Ű����� Ÿ���� lvalue ������ �߷еǾ��µ� ���� ���ø� �Ű����� Ÿ�Կ� ��������� float(lvalue)�� �־����� Ÿ���� ���� �ʾ� ����
		// ���� ������ lvalue�� �Ѱ�� �̴� ���ø� Ÿ���� lvalue ������ �߷еǾ��µ� ���� ��������� ���ø� Ÿ�Կ� lvalue�� �־ ������ �߻��� ��
		//std::tuple<float, float, float> element = std::make_tuple<float, float, float>(u, v, w); // error
		//std::tuple<float, float, float> element = std::make_tuple<float&, float&, float&>(u, v, w); // ok
		std::tuple<float, float, float> element = std::make_tuple(u, v, w); // ���ø� Ÿ���� �������� �����Ϸ��� �ڵ����� �߷��ϵ��� �ϴ� �͵� ���

		auto it = textureCoordinateMap.find(element);
		// �ؽ�ó ��ǥ�� ��ġ�ϴ� ������ ������ �ش� ���� �ε����� �Բ� map�� ����.
		if (it == textureCoordinateMap.end())
		{
			textureCoordinateMap.insert(std::make_pair(element, texVertexIndex));
		}
	}

	OptimizeTFace(pMesh, textureCoordinateMap);
}

void ASEParser::OptimizeTFace(ASE::Mesh* pMesh, std::map<std::tuple<float, float, float>, int>& textureCoordinateMap)
{
	for (int i = 0; i < pMesh->mFaceList.size(); ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			int textureVertexIndex = pMesh->mFaceList[i]->mTFace[j];

			float u = pMesh->mTVertexList[textureVertexIndex]->mU;
			float v = pMesh->mTVertexList[textureVertexIndex]->mV;
			float w = pMesh->mTVertexList[textureVertexIndex]->mW;

			std::tuple<float, float, float> uvwCoordinate = std::make_tuple(u, v, w);

			auto it = textureCoordinateMap.find(uvwCoordinate);
			assert(it != textureCoordinateMap.end());

			// �մܰ迡�� �ߺ��� �ؽ�ó ��ǥ�� ���� �����߱� ������(������ �����͸� ������ ���� �ƴϰ� map���ٰ� ����� �ε����� �߷� ���� ��)
			// ������ �ؽ�ó ���� �ε����� ����ϴ� ���� map�� �߷����� ������ �ε����� ��������
			pMesh->mFaceList[i]->mTFace[j] = it->second;
		}
	}
}

void ASEParser::SaveOptimizeData(ASE::Mesh* pMesh)
{
	pMesh->mOptimizedVertexList.resize(mDuplicateVertexHashTable.size());

	for (const auto& it : mDuplicateVertexHashTable)
	{
		int vertexIndex = it.second;

		ASE::Vertex* pVertex = new ASE::Vertex();
		pVertex->mPosition = std::get<0>(it.first);
		pVertex->mNormal = std::get<1>(it.first);
		pVertex->mTextureCoordinate = std::get<2>(it.first);

		pMesh->mOptimizedVertexList[vertexIndex] = pVertex;
	}

	for (int i = 0; i < pMesh->mFaceList.size(); ++i)
	{
		// face�� �̷�� ���� index�� int�� �Ľ�������,
		// index�� ������ ���ٴ� ���� ��� ���⵵ �ϰ�
		// IASetIndexBuffer()�� ȣ���� ���� DXGI_FORMAT�� R00_UINT�� ��ȣ ���� ������� �����ϱ� ������
		// UINT�� ĳ��������
		UINT vertexIndex0 = static_cast<UINT>(pMesh->mFaceList[i]->mVertexIndices[0]);

		// max�� ccw�� �ﰢ���� �����ϰ� dx�� cw�� �ﰢ���� �����ϱ� ������ 1��, 2�� �ε��� ������ ����
		UINT vertexIndex1 = static_cast<UINT>(pMesh->mFaceList[i]->mVertexIndices[2]);
		UINT vertexIndex2 = static_cast<UINT>(pMesh->mFaceList[i]->mVertexIndices[1]);

		// emplace_back�� Ÿ���� �����ϱ� ���� ���ڸ� �Ѱ��ָ� �Լ� ���ο��� �����ϸ鼭 container�� �־���.
		// ��, container�� �����͸� �߰��� �� ���ʿ��� �ӽ� ��ü ���� �� �Ҹ��� ���� �ʾƼ� �� �� ȿ������
		// push_back�� ��� �ӽ� ��ü�� �����ϰ� �Լ� ���ο��� ���ڷ� �Ѿ�� ��ü�� ������ ���� ����(lvalue�� ���) or
		// �̵� ����(rvalue�� �ѱ�鼭 �̵������ڰ� noexcept�� ���ǵǾ� �ִ� ���)�� �Ҹ��� �ӽ� ��ü�� �Ҹ��ϴ� �ܰ踦 ��ġ�� ������ �۾��� ����
		// emplace_back()�� ����,�Ҹ�, push_back()�� ����, ����/�̵� ����, �Ҹ�, �Ҹ�
		//pMesh->mOptimizedIndicesList.emplace_back(vertexIndex0, vertexIndex1, vertexIndex2);

		// ������ indices[3]���� ���� struct�� �Ѱܼ� ��ü ������ ����� �־��µ�
		// ������ ���� �ڷ����� UINT�� �ѱ�°Ŷ� ��ü ���� ����� ��� push_back()�̳� emplace_back()�̳� ���� ����
		pMesh->mOptimizedIndicesList.push_back(vertexIndex0);
		pMesh->mOptimizedIndicesList.push_back(vertexIndex1);
		pMesh->mOptimizedIndicesList.push_back(vertexIndex2);
		//pMesh->mOptimizedIndicesList.emplace_back(vertexIndex2);
	}
}

int ASEParser::ParsingNumberInt()
{
	mToken = mLexer->GetToken(mTokenStr);

	return atoi(mTokenStr);
}

const char* ASEParser::ParsingString()
{
	mToken = mLexer->GetToken(mTokenStr);

	return mTokenStr;
}

DirectX::XMFLOAT3 ASEParser::ParsingVector3()
{
	mToken = mLexer->GetToken(mTokenStr);
	float x = static_cast<float>(atof(mTokenStr));

	mToken = mLexer->GetToken(mTokenStr);
	float y = static_cast<float>(atof(mTokenStr));

	mToken = mLexer->GetToken(mTokenStr);
	float z = static_cast<float>(atof(mTokenStr));

	// DX�� y up �޼� ��ǥ�踦 ����ϰ�
	// 3ds Max�� z up ������ ��ǥ�踦 ����Ѵ�.
	// y�� z���� ��ȯ
	return DirectX::XMFLOAT3(x, z, y);
}

float ASEParser::ParsingNumberFloat()
{
	mToken = mLexer->GetToken(mTokenStr);

	return static_cast<float>(atof(mTokenStr));
}

void ASEParser::CreateOneMeshAndAddList()
{
	mOneMesh = new ASE::Mesh();
	mMeshList.push_back(mOneMesh);
}

void ASEParser::CreateOneVertexAndAddList()
{
	mOneVertex = new ASE::Vertex();
	mOneMesh->mVertexList.push_back(mOneVertex);
}

void ASEParser::CreateOneFaceAndAddList()
{
	mOneFace = new ASE::Face();
	mOneMesh->mFaceList.push_back(mOneFace);
}

void ASEParser::CreateOneTVertexAndAddList()
{
	mOneTVertex = new ASE::TVertex();
	mOneMesh->mTVertexList.push_back(mOneTVertex);
}
