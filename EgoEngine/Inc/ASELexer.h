#pragma once

#include <fstream>

// 2023 06 25 ������ home

// ���ڵ��� �����鼭 �ǹ� �ִ� ������ ��ū�� �����ϴ� ����

class ASELexer
{
public:
	ASELexer();
	~ASELexer();

public:
	bool OpenFile(const char* filePath);
	unsigned short GetToken(char* tokenStr);

private:
	char GetNextChar();

public:
	static constexpr unsigned int BUFFER_MAX_LENGTH = 65535;

	static constexpr unsigned short TOKEN_OPEN_BRACE = 50000;
	static constexpr unsigned short TOKEN_CLOSE_BRACE = 50001;

	static constexpr unsigned short TOKEN_NUMBER = 50002;
	static constexpr unsigned short TOKEN_STRING = 50003;

	static constexpr unsigned short TOKEN_END = 60000;

private:
	std::ifstream* mInputFileStream;
	char mFileBuffer[ASELexer::BUFFER_MAX_LENGTH];

	std::streamsize mNumCharExtracted;
	unsigned short mCurBufPos;
	unsigned long long mNumOfTokens;
};