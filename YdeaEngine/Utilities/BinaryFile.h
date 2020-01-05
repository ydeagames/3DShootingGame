/// <summary>
/// バイナリファイルを扱うクラス
/// </summary>
#pragma once

#include <memory>

class BinaryFile
{
public:
	// ファイル名を指定してロード
	static BinaryFile LoadFile(const wchar_t* fileName);

	BinaryFile();
	// ムーブコンストラクタ
	BinaryFile(BinaryFile&& in);

	// アクセッサ
	char* GetData() { return m_Data.get(); }
	unsigned int GetSize() { return m_Size; }

protected:
	// データ
	std::unique_ptr<char[]> m_Data;
	// サイズ
	unsigned int m_Size;
};
