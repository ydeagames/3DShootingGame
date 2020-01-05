#include "pch.h"
#include "BinaryFile.h"

#include <fstream>
#include <assert.h>


BinaryFile BinaryFile::LoadFile(const wchar_t * fileName)
{
	BinaryFile bin;

	std::ifstream ifs;

	// ファイルオープン
	ifs.open(fileName, std::ios::in | std::ios::binary);

	// 読み込み失敗時、強制終了
	assert(ifs);

	// ファイルサイズを取得
	ifs.seekg(0, std::fstream::end);
	std::streamoff eofPos = ifs.tellg();
	ifs.clear();
	ifs.seekg(0, std::fstream::beg);
	std::streamoff begPos = ifs.tellg();
	bin.m_Size = (unsigned int)(eofPos - begPos);

	// 読み込むためのメモリを確保
	bin.m_Data.reset(new char[bin.m_Size]);

	// ファイル先頭からバッファへコピー 
	ifs.read(bin.m_Data.get(), bin.m_Size);

	// ファイルクローズ
	ifs.close();

	return std::move(bin);
}

BinaryFile::BinaryFile()
{
	m_Size = 0;
}

BinaryFile::BinaryFile(BinaryFile && in)
{
	m_Data = std::move(in.m_Data);
	m_Size = in.m_Size;
}
