#include "EgoMatrix4x4.h"
#include <cassert>

EgoMath::EgoMatrix4x4::EgoMatrix4x4()
{
	m[0][0] = 1.f;
	m[0][1] = 0.f;
	m[0][2] = 0.f;
	m[0][3] = 0.f;

	m[1][0] = 0.f;
	m[1][1] = 1.f;
	m[1][2] = 0.f;
	m[1][3] = 0.f;

	m[2][0] = 0.f;
	m[2][1] = 0.f;
	m[2][2] = 1.f;
	m[2][3] = 0.f;

	m[3][0] = 0.f;
	m[3][1] = 0.f;
	m[3][2] = 0.f;
	m[3][3] = 1.f;
}

EgoMath::EgoMatrix4x4::EgoMatrix4x4(
	float _00, float _01, float _02, float _03,
	float _10, float _11, float _12, float _13,
	float _20, float _21, float _22, float _23,
	float _30, float _31, float _32, float _33)
{
	m[0][0] = _00;
	m[0][1] = _01;
	m[0][2] = _02;
	m[0][3] = _03;

	m[1][0] = _10;
	m[1][1] = _11;
	m[1][2] = _12;
	m[1][3] = _13;

	m[2][0] = _20;
	m[2][1] = _21;
	m[2][2] = _22;
	m[2][3] = _23;

	m[3][0] = _30;
	m[3][1] = _31;
	m[3][2] = _32;
	m[3][3] = _33;
}

EgoMath::EgoMatrix4x4::~EgoMatrix4x4()
{
}

float* EgoMath::EgoMatrix4x4::operator[](int x)
{
	assert(0 <= x && x < 4);

	return m[x];
}

float& EgoMath::EgoMatrix4x4::operator()(int x, int y)
{
	assert(0 <= x && x < 4);
	assert(0 <= y && y < 4);

	return m[x][y];
}

EgoMath::EgoMatrix4x4 EgoMath::EgoMatrix4x4::operator*(const EgoMatrix4x4& ref)
{
	EgoMath::EgoMatrix4x4 retMat;

	retMat[0][0] = m[0][0] * ref.m[0][0] + m[0][1] * ref.m[1][0] + m[0][2] * ref.m[2][0] + m[0][3] * ref.m[3][0];
	retMat[0][1] = m[0][0] * ref.m[0][1] + m[0][1] * ref.m[1][1] + m[0][2] * ref.m[2][1] + m[0][3] * ref.m[3][1];
	retMat[0][2] = m[0][0] * ref.m[0][2] + m[0][1] * ref.m[1][2] + m[0][2] * ref.m[2][2] + m[0][3] * ref.m[3][2];
	retMat[0][3] = m[0][0] * ref.m[0][3] + m[0][1] * ref.m[1][3] + m[0][2] * ref.m[2][3] + m[0][3] * ref.m[3][3];

	retMat[1][0] = m[1][0] * ref.m[0][0] + m[1][1] * ref.m[1][0] + m[1][2] * ref.m[2][0] + m[1][3] * ref.m[3][0];
	retMat[1][1] = m[1][0] * ref.m[0][1] + m[1][1] * ref.m[1][1] + m[1][2] * ref.m[2][1] + m[1][3] * ref.m[3][1];
	retMat[1][2] = m[1][0] * ref.m[0][2] + m[1][1] * ref.m[1][2] + m[1][2] * ref.m[2][2] + m[1][3] * ref.m[3][2];
	retMat[1][3] = m[1][0] * ref.m[0][3] + m[1][1] * ref.m[1][3] + m[1][2] * ref.m[2][3] + m[1][3] * ref.m[3][3];

	retMat[2][0] = m[2][0] * ref.m[0][0] + m[2][1] * ref.m[1][0] + m[2][2] * ref.m[2][0] + m[2][3] * ref.m[3][0];
	retMat[2][1] = m[2][0] * ref.m[0][1] + m[2][1] * ref.m[1][1] + m[2][2] * ref.m[2][1] + m[2][3] * ref.m[3][1];
	retMat[2][2] = m[2][0] * ref.m[0][2] + m[2][1] * ref.m[1][2] + m[2][2] * ref.m[2][2] + m[2][3] * ref.m[3][2];
	retMat[2][3] = m[2][0] * ref.m[0][3] + m[2][1] * ref.m[1][3] + m[2][2] * ref.m[2][3] + m[2][3] * ref.m[3][3];

	retMat[3][0] = m[3][0] * ref.m[0][0] + m[3][1] * ref.m[1][0] + m[3][2] * ref.m[2][0] + m[3][3] * ref.m[3][0];
	retMat[3][1] = m[3][0] * ref.m[0][1] + m[3][1] * ref.m[1][1] + m[3][2] * ref.m[2][1] + m[3][3] * ref.m[3][1];
	retMat[3][2] = m[3][0] * ref.m[0][2] + m[3][1] * ref.m[1][2] + m[3][2] * ref.m[2][2] + m[3][3] * ref.m[3][2];
	retMat[3][3] = m[3][0] * ref.m[0][3] + m[3][1] * ref.m[1][3] + m[3][2] * ref.m[2][3] + m[3][3] * ref.m[3][3];

	return retMat;
}

EgoMath::EgoMatrix4x4 EgoMath::EgoMatrix4x4::Identity()
{
	return EgoMatrix4x4(
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

EgoMath::EgoMatrix4x4 EgoMath::EgoMatrix4x4::MatrixMultiply(const EgoMatrix4x4& m1, const EgoMatrix4x4& m2)
{
	EgoMath::EgoMatrix4x4 retMat;

	retMat[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	retMat[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	retMat[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	retMat[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

	retMat[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	retMat[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	retMat[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	retMat[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

	retMat[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	retMat[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	retMat[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	retMat[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

	retMat[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	retMat[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	retMat[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	retMat[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];

	return retMat;
}
