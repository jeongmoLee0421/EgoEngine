#pragma once

// 2023 06 13 ������ home

// ���� ��Ʈ���� �ڷ����� �����Ͽ� interface�� ���� ����

// �ٱ��� EgoMatrix4x4�� �����Ͽ� interface�� �����ϰ�
// ���δ� XMMATRIX�� ����Ͽ� ���� �ӵ��� ����ȭ �� ��ǥ

namespace EgoMath
{
	class EgoMatrix4x4
	{
	public:
		EgoMatrix4x4();
		EgoMatrix4x4(
			float _00, float _01, float _02, float _03,
			float _10, float _11, float _12, float _13,
			float _20, float _21, float _22, float _23,
			float _30, float _31, float _32, float _33);
		~EgoMatrix4x4();

	public:
		float* operator[](int x);
		float& operator()(int x, int y);
		EgoMatrix4x4 operator*(const EgoMatrix4x4& ref);

	public:
		static EgoMatrix4x4 Identity();
		static EgoMatrix4x4 MatrixMultiply(const EgoMatrix4x4& m1, const EgoMatrix4x4& m2);

	private:
		float m[4][4];
	};
}

